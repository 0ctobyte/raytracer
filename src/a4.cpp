#include "a4.hpp"
#include "image.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <functional>
#include <thread>
#include <utility>

unsigned int progress = 0;
std::mutex progress_mut;
std::condition_variable progress_cond;

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

Colour a4_lighting(const Ray& ray, const Intersection& i, const std::shared_ptr<Light> light, const Point3D& light_pos)
{
  std::shared_ptr<const PhongMaterial> material = std::dynamic_pointer_cast<const PhongMaterial>(i.m);
  Point3D surface_point = i.q;
  Vector3D normal = material->bump(i.n, i.pu, i.pv, i.u, i.v);
  Colour material_diffuse = material->diffuse(i.u, i.v);
  Colour light_colour = light->getColour();
    
  // Set up the parameters for the lights
  // Calculate the vector from the surface point to the light source
  Vector3D surface_to_light = light_pos - surface_point;
  double distance_to_light = surface_to_light.length(); 
  surface_to_light.normalize();

  // Calculate the diffuse brightness
  // No need to divide dot product by product of the lengths since both vectors are normalized
  double diffuse_brightness = std::max(0.0, normal.dot(surface_to_light)); 

  // Calculate the diffuse colour component
  Colour diffuse = diffuse_brightness * material_diffuse * light_colour;

  // Calculate the vector from the eye point to the surface point
  Vector3D surface_to_eye = (ray.origin() - surface_point).normalized();

  // Calculate the halfway vector according to the Blinn-Phong lighing model
  Vector3D halfway = (surface_to_light + surface_to_eye).normalized();

  // Calculate the specular brightness
  // Can't have specular highlights if no diffuse lighting at the point!
  double specular_brightness = (diffuse_brightness > 0) ? pow(std::max(0.0, normal.dot(halfway)), material->shininess()) : 0.0;

  // Calculate the specular colour component
  Colour specular = specular_brightness * material->specular() * light_colour;

  return light->getAttenuation(distance_to_light) * (diffuse + specular);
}

Colour a4_shadow_ray(const Ray& ray, const std::shared_ptr<SceneNode> root, const std::shared_ptr<Light> light, const Point3D& light_pos, const Point3D& hit, const Intersection& i)
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

Ray a4_reflect(const Point3D& origin, const Vector3D& direction, const Vector3D& normal)
{
  return Ray(origin, direction - 2*direction.dot(normal)*normal);
}

std::tuple<bool, double, Ray>   a4_refract(const Point3D& hit, const Vector3D& direction, const Vector3D& n, double ni)
{
  Ray refracted_ray(Point3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0));
  double R = 1.0;
  
  Vector3D normal = n;
  double cosI = direction.dot(normal);
  double n1, n2;

  if(cosI > 0)
  {
    // direction and normal are in the same direction so ray originates from inside primitive
    n1 = ni;
    n2 = 1.0;
    normal = -normal;
  }
  else
  {
    // Ray enters primitive from outside
    n1 = 1.0;
    n2 = ni;
    cosI = -cosI;
  }

  // Check for total internal reflection
  double nr = n1 / n2;
  double cosT = 1.0 - (nr * nr) * (1.0 - (cosI * cosI));
  if(cosT >= 0)
  {
    cosT = sqrt(cosT);

    // Compute Fresnel coefficient
    double a = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
    double b = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
    R = ((a * a) + (b * b)) * 0.5;

    // Cast the refracted ray
    refracted_ray = Ray(hit - (1e-9) * normal, nr * direction + (nr * cosI - cosT) * normal);
  }

  return std::tuple<bool, double, Ray>(cosT < 0, R, refracted_ray);
}

Colour a4_trace_ray(const Ray& ray, const std::shared_ptr<SceneNode> root, const std::list<std::shared_ptr<Light>>& lights, const Colour& ambient, const Colour& bg, const std::function<double()>& uniform, unsigned int recurse_level, unsigned int shadow_samples)
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
    std::shared_ptr<const PhongMaterial> material = std::dynamic_pointer_cast<const PhongMaterial>(i.m);
    Colour diffuse = material->diffuse(i.u, i.v);
    colour = ambient * diffuse;

    if(diffuse != Colour(0.0, 0.0, 0.0))
    {
      for(auto light : lights)
      {
        // Cast shadow rays to each light source (multiple times if area light for soft shadows)
        Colour shade_colour(0.0, 0.0, 0.0);
        unsigned int num_shadow_rays = (light->isPointLight()) ? 1 : shadow_samples;
        for(unsigned int j = 0; j < num_shadow_rays; j++)
        {
          Point3D light_pos = (num_shadow_rays == 1) ? light->getPosition() : light->getPosition(uniform);
          shade_colour = shade_colour + a4_shadow_ray(ray, root, light, light_pos, hit, i);
        }
        if(shade_colour == Colour(0.0, 0.0, 0.0)) continue;
        colour = colour + Colour(shade_colour.R() / num_shadow_rays, shade_colour.G() / num_shadow_rays, shade_colour.B() / num_shadow_rays);
      }
    }

    // Cast reflection rays and add the colour returned to render reflections on object
    Colour reflected_colour(0.0, 0.0, 0.0);
    if(material->specular() != Colour(0.0, 0.0, 0.0) && recurse_level > 0) 
    {
      Ray reflected_ray = a4_reflect(hit, ray.direction(), n);
      reflected_colour = a4_trace_ray(reflected_ray, root, lights, ambient, reflected_colour, uniform, --recurse_level, shadow_samples);
    }

    // Cast refracted rays and add the colour returned
    double R = 1.0;
    Colour refracted_colour(0.0, 0.0, 0.0);
    if(material->ni() > 0 && recurse_level > 0)
    {
      std::tuple<bool, double, Ray> ret = a4_refract(i.q, ray.direction(), n, material->ni());
      bool total_internal_reflection = std::get<0>(ret);
      R = std::get<1>(ret);
      if(!total_internal_reflection)
      {
        Ray refracted_ray = std::get<2>(ret);
        refracted_colour = a4_trace_ray(refracted_ray, root, lights, ambient, refracted_colour, uniform, --recurse_level, shadow_samples);
      }
    }

    // Add the reflection and refraction components and scale them with the Fresnel coefficient
    colour = colour + material->specular() * ((R * reflected_colour) + ((1 - R) * refracted_colour));
  }

  return colour;
}

Colour a4_get_background_colour(Image& img, int x, int y, int width, int height)
{
  double u = (double)x / (double)width;
  double v = (double)y / (double)height;
  
  double di = u * (double)(img.width()-1);
  double dj = v * (double)(img.height()-1);

  int i = di;
  int j = dj;

  // Wrap around if the coordinates are out of bounds
  int i1 = ((i+1) >= img.width()) ? 0 : i+1;
  int j1 = ((j+1) >= img.height()) ? 0 : j+1;

  // Get the barycentric coordinates between pixels
  double up = di - i;
  double vp = dj - j;

  // Bilinear interpolation between pixel values, basically a weighted average of the sourrounding pixels
  // using the barycentric coordinates up, vp
  return (1-up)*(1-vp)*Colour(img(i, j, 0), img(i, j, 1), img(i, j, 2)) +
    (1-up)*(vp)*Colour(img(i, j1, 0), img(i, j1, 1), img(i, j1, 2)) +
    (up)*(1-vp)*Colour(img(i1, j, 0), img(i1, j, 1), img(i1, j, 2)) +
    (up)*(vp)*Colour(img(i1, j1, 0), img(i1, j1, 1), img(i1, j1, 2));
}

void a4_render_thread(Image* img, unsigned int ystart, unsigned int yskip, unsigned int width, unsigned int height, std::shared_ptr<SceneNode> root, const Matrix4x4 unproject, const Point3D eye, const Colour ambient, const std::list<std::shared_ptr<Light>> lights, unsigned int reflection_level, unsigned int aa_samples, unsigned int shadow_samples, Image* bgimg)
{
  // Seed the rng and set the uniform distribution object
  std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<double> uniform_distribution(0, 1);
  std::function<double()> uniform = std::bind(uniform_distribution, std::ref(gen));

  int one_percent = width * height * 0.01;
  int pixel_count = 0;

  shadow_samples = (shadow_samples == 0) ? 1 : shadow_samples;
  aa_samples = (aa_samples == 0) ? 1 : aa_samples;
  for (unsigned int y = ystart; y < height; y += yskip) {
    for (unsigned int x = 0; x < width; x++) {
      // Background colour. a4_trace_ray returns this if no intersections
      Colour bg = bgimg->empty() ? ((x+y) & 0x10) ? (double)y/height * Colour(1.0, 1.0, 1.0) : Colour(0.0, 0.0, 0.0) :
        a4_get_background_colour(*bgimg, x, y, img->width(), img->height());

      // Cast a ray into the scene and get the colour returned
      Colour colour(0.0, 0.0, 0.0);

      // For antialiasing, divide the "pixel" into a n by n grid and cast rays from a random point within each grid box
      for(unsigned int p = 0; p < aa_samples; p++)
      {
        for(unsigned int q = 0; q < aa_samples; q++)
        {
          // Unproject the pixel to the projection plane
          double e = uniform();
          Point3D pixel ((double)x + ((double)p + e) / (double)aa_samples, (double)y - ((double)q + e) / (double)aa_samples, 0.0);
          Point3D p = unproject * pixel;

          // Create the ray with origin at the eye point
          Ray ray(eye, p-eye);

          colour = colour + a4_trace_ray(ray, root, lights, ambient, bg, uniform, reflection_level, shadow_samples);
        }
      }

      // Of course, have to divide the colour by the number of samples taken
      double n = aa_samples * aa_samples;
      colour = Colour(colour.R() / n, colour.G() / n, colour.B() / n);

      (*img)(x, y, 0) = colour.R();
      (*img)(x, y, 1) = colour.G();
      (*img)(x, y, 2) = colour.B();

      if(++pixel_count >= one_percent)
      {
        {
          std::lock_guard<std::mutex> lock(progress_mut);
          progress++;
        }
        progress_cond.notify_one();
        
        pixel_count = 0;
      }
    }
  }
}

void a4_render(// What to render
               std::shared_ptr<SceneNode> root,
               // Where to output the image
               const std::string& filename,
               // Image size
               unsigned int width, unsigned int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<std::shared_ptr<Light>>& lights,
               // Optional parameters: Reflection recursive level, antialiasing samples
               unsigned int num_threads,
               unsigned int reflection_level,
               unsigned int aa_samples,
               unsigned int shadow_samples,
               const std::string& bgfilename
               )
{
  // Fill in raytracing code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  std::cerr << "Stub: a4_render(" << root.get() << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";

  for (std::list<std::shared_ptr<Light>>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << *I->get();
  }
  std::cerr << ", " << num_threads << ", " << reflection_level << ", " << aa_samples << ", " << shadow_samples;
  std::cerr << ", " << bgfilename << "});" << std::endl;

  // Open the background image file if one is given
  Image bg;
  if(!bgfilename.empty()) bg.loadPng(bgfilename);

  // Get pixel unprojection matrix
  double d = view.length();
  Matrix4x4 unproject = a4_get_unproject_matrix(width, height, fov, d, eye, view, up);
    
  Image img(width, height, 3);

  // Flatten the scene hierarchy
  root->flatten();

  if(num_threads == 0) num_threads = 1;

  std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << ", Threads requested: " << num_threads << std::endl;

  std::vector<std::thread> threads(num_threads);
  for(unsigned int i = 0; i < num_threads; i++)
  {
    threads[i] = std::thread(a4_render_thread, &img, i, num_threads, width, height, root, unproject, eye, ambient, lights, reflection_level, aa_samples, shadow_samples, &bg);
    if(threads[i].get_id() == std::thread::id())
    {
      std::cerr << "Abort: Failed to create thread " << i << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // Get dimensions of terminal
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

  std::unique_lock<std::mutex> lock(progress_mut);
  while(progress != 100)
  {
    progress_cond.wait(lock);
    std::chrono::duration<double> duration = std::chrono::system_clock::now() - start;
    int w = ws.ws_col-35;
    int c = (float)progress/100.0 * w;
    int min = duration.count() / 60.0;
    int hours = (double)min / 60.0;
    std::cout << "Progress: " << progress << "% [";
    for(int i = 0; i < c; i++) std::cout << "=";
    for(int i = c; i < w; i++) std::cout << " ";
    std::cout << "] " << hours << "h" << min - (hours*60.0) << "m" << duration.count() - (min*60.0) << "s" << "\r" << std::flush;
  }
  std::cout << std::endl;

  for(unsigned int i = 0; i < num_threads; i++) threads[i].join();

  img.savePng(filename);
  
}
