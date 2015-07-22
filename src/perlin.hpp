#ifndef CS488_PERLIN_HPP
#define CS488_PERLIN_HPP

#include <array>

#define HASH_P (0xff)
#define HASH_G (0xf)

class Perlin {
public:
  static void init();
  static double noise(double x, double y, double z);
  static double turbulence(double x, double y, double z, unsigned octaves, double freq, double gain);

  static double marble(double x, double y, double z);
  static double wood(double x, double y, double z);
  static double cloud(double x, double y, double z);

private: 
  static std::array<int, HASH_P+HASH_P+1> P; 
};

#endif // CS488_PERLIN_HPP

