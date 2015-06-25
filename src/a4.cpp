#include "a4.hpp"
#include "image.hpp"

#include <cmath>
#include <algorithm>

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

Colour a4_lighting(const Ray& ray, const Intersection& i, const Light* light, const Colour& ambient, const Colour& reflected_colour, bool blocked)
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

  // Caluclate the reflection color
  Colour reflection = material->specular() * reflected_colour; 

  // Calculate attenuation factor
  double attenuation = 1.0 / (light->falloff[0] + light->falloff[1]*distance_to_light + light->falloff[2]*(distance_to_light*distance_to_light));

  Colour lighted = ambient * material->diffuse() + (int)(!blocked) * attenuation * (diffuse + specular + reflection);

  return lighted;
}

Colour a4_trace_ray(const Ray& ray, const SceneNode *root, const Light* light, const Colour& ambient, const Colour& bg, int recurse_level)
{
  // Test intersection of ray with scene for each light source
  Colour colour = bg;
  Intersection i;

  bool intersected = root->intersect(ray, i);

  if(intersected)
  {
    // Cast shadow rays to the light source. If the ray intersects an object before reaching the light
    // source then don't count that light sources contribution since it is being blocked
    // Move the hit position a little away from the object so the ray doesn't intersect from the originating object
    Point3D hit = i.q + (1e-9)*i.n;
    Ray shadow(hit, light->position-hit);
    Intersection u;
    bool blocked = root->intersect(shadow, u);

    // Make sure to check if intersection point is before light source
    if((u.q - shadow.origin()).length() > (light->position-shadow.origin()).length()) blocked = false;

    Colour reflected_colour(0.0, 0.0, 0.0);
    if(recurse_level > 0) 
    {
      Ray reflected_ray(hit, ray.direction() - 2*ray.direction().dot(i.n)*i.n);
      reflected_colour = a4_trace_ray(reflected_ray, root, light, ambient, reflected_colour, --recurse_level);
    }

    // Perform phong shading at intersection point
    colour = a4_lighting(ray, i, light, ambient, reflected_colour, blocked);
  }

  return colour;
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
    
  // For now, just make a sample image.

  Image img(width, height, 3);

  int one_percent = width*height*0.01;
  int pixel_count = 0, percentage = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Unproject the pixel to the projection plane
      Point3D pixel (x, y, 0.0);
      Point3D p = unproject * pixel;

      // Create the ray with origin at the eye point
      Ray ray(eye, p-eye);

      // Background colour. a4_trace_ray returns this if no intersections
      Colour bg = ((x+y) & 0x10) ? (double)y/height * Colour(1.0, 1.0, 1.0) : Colour(0.0, 0.0, 0.0);

      // Cast rays for each light that is in the scene
      // Accumulate the colours returned
      Colour colour(0.0, 0.0, 0.0);
      for(auto light : lights) colour = colour + a4_trace_ray(ray, root, light, ambient, bg, 1);
      
      img(x, y, 0) = colour.R();
      img(x, y, 1) = colour.G();
      img(x, y, 2) = colour.B();

      if(++pixel_count >= one_percent)
      {
        pixel_count = 0;
        std::cout << "progress: " << ++percentage << "% \r" << std::flush;
      }
    }
  }
    
  std::cout << std::endl;

  img.savePng(filename);
  
}
