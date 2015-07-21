#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include <memory>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

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
               // Optional parameters: Recursion level, antialiasing samples, shadow rays, background image
               unsigned int num_threads,
               unsigned int recurse_level,
               unsigned int aa_samples,
               unsigned int shadow_samples,
               unsigned int glossy_samples,
               const std::string& bgfilename
               );

#endif
