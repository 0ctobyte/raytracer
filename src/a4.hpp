#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               unsigned int width, unsigned int height,
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
               );

#endif
