#include "a4.hpp"
#include "image.hpp"

#include <pthread.h>
#include <cmath>
#include <algorithm>

struct RenderThreadData {
  Image& img;
  int ystart, yskip, width, height;
  SceneNode* root;
  Matrix4x4 unproject;
  Point3D eye;
  Colour ambient;
  std::list<Light*> lights;
  int* progress;
  bool* done;
};

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

Colour a4_lighting(const Ray& ray, const Intersection& i, const Light* light)
{
  Point3D surface_point = i.q;
  Vector3D normal = i.n;
  const PhongMaterial *material = dynamic_cast<const PhongMaterial*>(i.m);
    
  // Set up the parameters for the lights
  // Calculate the vector from the surface point to the light source
  Vector3D surface_to_light = light->position - surface_point;
  double distance_to_light = surface_to_light.length(); 
  surface_to_light.normalize();

  // Calculate the diffuse brightness
  // No need to divide dot product by product of the lengths since both vectors are normalized
  double diffuse_brightness = std::max(0.0, normal.dot(surface_to_light)); 

  // Calculate the diffuse colour component
  Colour diffuse = diffuse_brightness * material->diffuse() * light->colour;

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
  Colour specular = specular_brightness * material->specular() * light->colour;

  // Calculate attenuation factor
  double attenuation = 1.0 / (light->falloff[0] + light->falloff[1]*distance_to_light + light->falloff[2]*(distance_to_light*distance_to_light));

  return attenuation * (diffuse + specular);
}

Colour a4_trace_ray(const Ray& ray, const SceneNode *root, const std::list<Light*>& lights, const Colour& ambient, const Colour& bg, int recurse_level)
{
  // Test intersection of ray with scene for each light source
  Colour colour = bg;
  Intersection i;

  bool intersected = root->intersect(ray, i);

  if(intersected)
  {
    // Calculate hit point. Move the hit position a little away from the object so the ray doesn't intersect from the originating object
    Point3D hit = i.q + (1e-9)*i.n;

    // Add the ambient colour to the object
    const PhongMaterial* material = dynamic_cast<const PhongMaterial*>(i.m);
    colour = ambient * material->diffuse();

    for(auto light : lights)
    {
      // Cast shadow rays to the light source. If the ray intersects an object before reaching the light
      // source then don't count that light sources contribution since it is being blocked
      Ray shadow(hit, light->position-hit);
      Intersection u;
      
      // Make sure to check if intersection point is before light source
      if(root->intersect(shadow, u) && (u.q-shadow.origin()).length() < (light->position-shadow.origin()).length()) continue;

      // Perform phong shading at intersection point. The ambient factor is essentially 1 / number of lights.
      // This is so that the ambient light is not added to the final colour multiple times (one time for each light source)
      colour = colour + a4_lighting(ray, i, light);
    }

    // Cast reflection rays and add the colour returned to render reflections on object
    Colour reflected_colour(0.0, 0.0, 0.0);
    if(recurse_level > 0) 
    {
      Ray reflected_ray(hit, ray.direction() - 2*ray.direction().dot(i.n)*i.n);
      reflected_colour = a4_trace_ray(reflected_ray, root, lights, ambient, reflected_colour, --recurse_level);
    }

    // Add the reflection. A coefficient is multiplied with the colour to damp the saturation due to multiple light sources
    colour = colour + (1.0 / lights.size()) * reflected_colour * material->specular();
  }

  return colour;
}

void* a4_render_thread(void* data)
{
  RenderThreadData d = *static_cast<RenderThreadData*>(data);

  int one_percent = d.width * d.height * 0.01;
  int pixel_count = 0, percentage = 0;

  for (int y = d.ystart; y < d.height; y += d.yskip) {
    for (int x = 0; x < d.width; x++) {
      // Unproject the pixel to the projection plane
      Point3D pixel (x, y, 0.0);
      Point3D p = d.unproject * pixel;

      // Create the ray with origin at the eye point
      Ray ray(d.eye, p-d.eye);

      // Background colour. a4_trace_ray returns this if no intersections
      Colour bg = ((x+y) & 0x10) ? (double)y/d.height * Colour(1.0, 1.0, 1.0) : Colour(0.0, 0.0, 0.0);

      // Cast a ray into the scene and get the colour returned
      Colour colour(0.0, 0.0, 0.0);
      colour = a4_trace_ray(ray, d.root, d.lights, d.ambient, bg, 1);
      
      d.img(x, y, 0) = colour.R();
      d.img(x, y, 1) = colour.G();
      d.img(x, y, 2) = colour.B();

      if(++pixel_count >= one_percent)
      {
        pixel_count = 0;
        *d.progress = ++percentage;
      }
    }
  }
  *d.done = true;

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
               const std::list<Light*>& lights
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
  std::cerr << "});" << std::endl;

  // Get pixel unprojection matrix
  double d = view.length();
  Matrix4x4 unproject = a4_get_unproject_matrix(width, height, fov, d, eye, view, up);
    
  Image img(width, height, 3);

  int progress1 = 0, progress2 = 0, progress3 = 0, progress4 = 0;
  bool done1 = false, done2 = false, done3 = false, done4 = false;
  RenderThreadData data1 = {img, 0, 4, width, height, root, unproject, eye, ambient, lights, &progress1, &done1};
  RenderThreadData data2 = {img, 1, 4, width, height, root, unproject, eye, ambient, lights, &progress2, &done2};
  RenderThreadData data3 = {img, 2, 4, width, height, root, unproject, eye, ambient, lights, &progress3, &done3};
  RenderThreadData data4 = {img, 3, 4, width, height, root, unproject, eye, ambient, lights, &progress4, &done4};

  pthread_t t1, t2, t3, t4;

  int ret = pthread_create(&t1, NULL, a4_render_thread, &data1);
  if(ret)
  {
    std::cerr << "Abort: pthread_create failed with error code: " << ret << std::endl;
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&t2, NULL, a4_render_thread, &data2);
  if(ret)
  {
    std::cerr << "Abort: pthread_create failed with error code: " << ret << std::endl;
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&t3, NULL, a4_render_thread, &data3);
  if(ret)
  {
    std::cerr << "Abort: pthread_create failed with error code: " << ret << std::endl;
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&t4, NULL, a4_render_thread, &data4);
  if(ret)
  {
    std::cerr << "Abort: pthread_create failed with error code: " << ret << std::endl;
    exit(EXIT_FAILURE);
  }

  while(!done1 || !done2 || !done3 || !done4)
  {
    std::cout << "progress: " << progress1+progress2+progress3+progress4 << "% \r" << std::flush;
  }
  std::cout << std::endl;
  
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  img.savePng(filename);
  
}
