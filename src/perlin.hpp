#ifndef CS488_PERLIN_HPP
#define CS488_PERLIN_HPP

#include "algebra.hpp"
#include <array>

#define HASH_P (0xff)
#define HASH_G (0xf)

class Perlin {
public:
  static void init();
  static double noise(double x, double y, double z);

  static double marble(double x, double y, double z);
  static double wood(double x, double y, double z);

private: 
  static std::array<int, HASH_P+HASH_P+1> P; 
  static std::array<Vector3D, HASH_G+1> G;
};

#endif // CS488_PERLIN_HPP

