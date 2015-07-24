# Ray Tracer

A ray tracer implemented in C++. It traverses a scene file described by a Lua based DSL.
This was written for a project for the CS 488 course at the University of Waterloo.

Check out the [site](http://0ctobyte.github.io/raytracer/) to see some of the images rendered with
the ray tracer!

## Build & Usage

Run `make` in the src directory to compile the program.

To run the program (`<scene>` is the lua scene file):

`./rt <scene>.lua`

## Features
* Standard Primitives
  - Sphere
  - Cube
  - Polygonal Mesh (faces are assumed to be convex and planar)
  - Cone
  - Cylinder
  - Torus
  - Disc
* Constructive Solid Geometry
* Soft Shadows
* Anti-Aliasing
* Texture Mapping
* Bump Mapping
* Refraction
* Phong Shading (Normal Interpolation)
* Perlin Noise
* Glossy Reflection

