#include "a4.hpp"
#include "image.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>

struct RenderThreadData {
  Image* img;
  int ystart, yskip, width, height;
  SceneNode* root;
  Matrix4x4 unproject;
  Point3D eye;
  Colour ambient;
  std::list<Light*> lights;
  unsigned int reflection_level, aa_samples;
};

unsigned int progress = 0;
pthread_mutex_t progress_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t progress_cond = PTHREAD_COND_INITIALIZER;

Matrix4x4 a4_get_unproject_matrix(int width, int height, double fov, double d, Point3D eye, Vector3D view, Vector3D up)
{
  double fov_r = fov * M_PI / 180.0;
  double h = 2.0*d*tan(fov_r / 2.0); // height of projection plane based field of view and distance to the plane
  
  // First translate the pixel so that it is centered at the origin in the projection plane (origin is in the middle of the screen)
  Matrix4x4 viewport_translate = Matrix4x4().translate(-(double)width / 2.0, -(double)height / 2.0, d);

  // Then scale it to the projection plane such that aspect ratio is maintained and we have a right handed coordinate system
  Matrix4x4 viewport_scale = Matrix4x4().scale(-h / (double)height, -h / (double)height, 1.0);

  // Calculate the basis for the view coordinate system
  view.normalize();
  up.normalize();
  Vector3D u = up.cross(view);
  u.normalize();
  Vector3D v = view.cross(u);
  v.normalize();

  // Create the view rotation and translation matrix
  Matrix4x4 view_rotate = Matrix4x4(Vector4D(u, 0.0), Vector4D(v, 0.0), Vector4D(view, 0.0), Vector4D(0.0, 0.0, 0.0, 1.0)).transpose();
  Matrix4x4 view_translate = Matrix4x4().translate(Vector3D(eye));

  // Now multiply these together to form the pixel to 3D point transformation matrix
  Matrix4x4 unproject = view_translate * view_rotate * viewport_scale * viewport_translate;

  return unproject;
}

Colour a4_lighting(const Ray& ray, const Intersection& i, const Light* light, const Point3D& light_pos)
{
  Point3D surface_point = i.q;
  Vector3D normal = i.n.normalized();
  const PhongMaterial *material = dynamic_cast<const PhongMaterial*>(i.m);
    
  // Set up the parameters for the lights
  // Calculate the vector from the surface point to the light source
  Vector3D surface_to_light = light_pos - surface_point;
  double distance_to_light = surface_to_light.length(); 
  surface_to_light.normalize();

  // Calculate the diffuse brightness
  // No need to divide dot product by product of the lengths since both vectors are normalized
  double diffuse_brightness = std::max(0.0, normal.dot(surface_to_light)); 

  // Calculate the diffuse colour component
  Colour diffuse = diffuse_brightness * material->diffuse() * light->getColour();

  // Calculate the angle of reflectance
  // The incidence vector is the vector surface_to_light but in the opposite direction
  Vector3D incidence = -surface_to_light;
  Vector3D reflected = (incidence - 2*(incidence.dot(normal))*normal).normalized();

  // Calculate the vector from the eye point to the surface point
  Vector3D surface_to_eye = (ray.origin() - surface_point).normalized();

  // Calculate the specular brightness
  // Can't have specular highlights if no diffuse lighting at the point!
  double specular_brightness = (diffuse_brightness > 0) ? pow(std::max(0.0, surface_to_eye.dot(reflected)), material->shininess()) : 0.0;

  // Calculate the specular colour component
  Colour specular = specular_brightness * material->specular() * light->getColour();

  return light->getAttenuation(distance_to_light) * (diffuse + specular);
}

Colour a4_shadow_ray(const Ray& ray, const SceneNode* root, const Light* light, const Point3D& light_pos, const Point3D& hit, const Intersection& i)
{
  // Cast shadow rays to the light source. If the ray intersects an object before reaching the light
  // source then don't count that light sources contribution since it is being blocked
  Ray shadow(hit, light_pos-hit);
  Intersection u;
  
  // Make sure to check if intersection point is before light source
  if(root->intersect(shadow, u) && (u.q-shadow.origin()).length() < (light_pos-shadow.origin()).length()) return Colour(0.0, 0.0, 0.0);

  // Perform phong shading at intersection point. The ambient factor is essentially 1 / number of lights.
  // This is so that the ambient light is not added to the final colour multiple times (one time for each light source)
  return a4_lighting(ray, i, light, light_pos);
}

Colour a4_trace_ray(const Ray& ray, const SceneNode* root, const std::list<Light*>& lights, const Colour& ambient, const Colour& bg, unsigned int recurse_level, unsigned int samples)
{
  // Test intersection of ray with scene for each light source
  Colour colour = bg;
  Intersection i;

  bool intersected = root->intersect(ray, i);

  if(intersected)
  {
    // Calculate hit point. Move the hit position a little away from the object so the ray doesn't intersect from the originating object
    Vector3D n = i.n.normalized();
    Point3D hit = i.q + (1e-9)*n;

    // Add the ambient colour to the object
    const PhongMaterial* material = dynamic_cast<const PhongMaterial*>(i.m);
    colour = ambient * material->diffuse();

    for(auto light : lights)
    {
      // Cast shadow rays to each light source
      Point3D light_pos = light->getPosition();
      colour = colour + a4_shadow_ray(ray, root, light, light_pos, hit, i);
    }

    // Cast reflection rays and add the colour returned to render reflections on object
    Colour reflected_colour(0.0, 0.0, 0.0);
    if(recurse_level > 0) 
    {
      Ray reflected_ray(hit, ray.direction() - 2*ray.direction().dot(n)*n);
      reflected_colour = a4_trace_ray(reflected_ray, root, lights, ambient, reflected_colour, --recurse_level, samples);
    }

    // Add the reflection. A coefficient is multiplied with the colour to damp the saturation due to multiple light sources
    colour = colour + (reflected_colour * material->specular());
  }

  return colour;
}

void* a4_render_thread(void* data)
{
  RenderThreadData d = *static_cast<RenderThreadData*>(data);

  int one_percent = d.width * d.height * 0.01;
  int pixel_count = 0;

  int samples = (d.aa_samples == 0) ? 1 : d.aa_samples;
  for (int y = d.ystart; y < d.height; y += d.yskip) {
    for (int x = 0; x < d.width; x++) {
      // Background colour. a4_trace_ray returns this if no intersections
      Colour bg = ((x+y) & 0x10) ? (double)y/d.height * Colour(1.0, 1.0, 1.0) : Colour(0.0, 0.0, 0.0);

      // Cast a ray into the scene and get the colour returned
      Colour colour(0.0, 0.0, 0.0);

      // For antialiasing, divide the "pixel" into a n by n grid and cast rays from a random point within each grid box
      for(int p = 0; p < samples; p++)
      {
        for(int q = 0; q < samples; q++)
        {
          // Unproject the pixel to the projection plane
          double e = rand() / RAND_MAX;
          Point3D pixel ((double)x + ((double)p + e) / (double)samples, (double)y - ((double)q + e) / (double)samples, 0.0);
          Point3D p = d.unproject * pixel;

          // Create the ray with origin at the eye point
          Ray ray(d.eye, p-d.eye);

          colour = colour + a4_trace_ray(ray, d.root, d.lights, d.ambient, bg, d.reflection_level, d.aa_samples);
        }
      }

      // Of course, have to divide the colour by the number of samples taken
      double n = samples * samples;
      colour = Colour(colour.R() / n, colour.G() / n, colour.B() / n);

      (*d.img)(x, y, 0) = colour.R();
      (*d.img)(x, y, 1) = colour.G();
      (*d.img)(x, y, 2) = colour.B();

      if(++pixel_count >= one_percent)
      {
        pixel_count = 0;

        pthread_mutex_lock(&progress_mut);
        progress++;
        pthread_cond_signal(&progress_cond);
        pthread_mutex_unlock(&progress_mut);
      }
    }
  }

  return NULL;
}

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights,
               // Optional parameters: Reflection recursive level, antialiasing samples
               unsigned int num_threads,
               unsigned int reflection_level,
               unsigned int aa_samples,
               unsigned int shadow_samples
               )
{
  // Fill in raytracing code here.

  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << ", " << num_threads << ", " << reflection_level << ", " << aa_samples << ", " << shadow_samples << "});" << std::endl;

  // Seed the rng
  srand(time(NULL));

  // Get pixel unprojection matrix
  double d = view.length();
  Matrix4x4 unproject = a4_get_unproject_matrix(width, height, fov, d, eye, view, up);
    
  Image img(width, height, 3);

  if(num_threads == 0) num_threads = 1;

  std::vector<pthread_t> threads(num_threads);
  std::vector<RenderThreadData> renderData(num_threads);
  for(unsigned int i = 0; i < num_threads; i++)
  {
    renderData[i] = (RenderThreadData){&img, i, num_threads, width, height, root, unproject, eye, ambient, lights, reflection_level, aa_samples};
    int ret = pthread_create(&threads[i], NULL, a4_render_thread, &renderData[i]);
    if(ret)
    {
      std::cerr << "Abort: pthread_create (thread " << i << ") failed with error code: " << ret << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // Get dimensions of terminal
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

  pthread_mutex_lock(&progress_mut);
  while(progress != 100)
  {
    pthread_cond_wait(&progress_cond, &progress_mut);
    int w = ws.ws_col-17;
    int c = (float)progress/100.0 * w;
    std::cout << "Progress: " << progress << "% [";
    for(int i = 0; i < c; i++) std::cout << "=";
    for(int i = c; i < w; i++) std::cout << " ";
    std::cout << "]" << "\r" << std::flush;
  }
  pthread_mutex_unlock(&progress_mut);
  std::cout << std::endl;

  for(unsigned int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

  img.savePng(filename);
  
}
