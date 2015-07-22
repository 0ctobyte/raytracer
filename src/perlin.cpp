#include "perlin.hpp"

#include <cmath>

std::array<int, HASH_P+HASH_P+1> Perlin::P;

void Perlin::init()
{
  // Hash lookup table as defined by Ken Perlin.  This is a randomly
  // arranged array of all numbers from 0-255 inclusive.
  int permutation[] = { 151,160,137,91,90,15,                 
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
  };

  // Insert random values into the hash
  for(int i = 0; i < (HASH_P + HASH_P + 1); i++) P[i] = permutation[i & HASH_P];
}

double Perlin::noise(double x, double y, double z) 
{
  auto fade = [](double t) -> double {
    return t * t * t * (t * (t * 6 - 15) + 10);
  };

  auto lerp = [](double a, double b, double t) -> double {
    return a + t * (b - a);
  };

  auto grad = [](int hash, double x, double y, double z) {
    // Convert low 4 bits of hash into 12 gradient vectors
    int h = hash & HASH_G;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  };

  // Get the nearest point smaller than the given point in in the unit integer lattice
  // And scale it so that it can index the permuation table
  int x0 = (long long)std::floor(x) & HASH_P;
  int y0 = (long long)std::floor(y) & HASH_P;
  int z0 = (long long)std::floor(z) & HASH_P;

  // Get the fractional parts of the point coordinates
  double xf = x - std::floor(x);
  double yf = y - std::floor(y);
  double zf = z - std::floor(z);

  // Calculated the splined interpolation values of the point in the unit lattice
  double u = fade(xf);
  double v = fade(yf);
  double w = fade(zf);

  // Get hash coordinates of the 8 unit cube corners
  int A = P[x0]+y0, AA = P[A]+z0, AB = P[A+1]+z0;
  int B = P[x0+1]+y0, BA = P[B]+z0, BB = P[B+1]+z0;

  // Trilinearly interpolate the influence values
  return lerp(
      lerp(
        lerp(grad(P[AA], xf, yf, zf), grad(P[BA], xf-1, yf, zf), u), 
        lerp(grad(P[AB], xf, yf-1, zf), grad(P[BB], xf-1, yf-1, zf), u), 
        v), 
      lerp(
        lerp(grad(P[AA+1], xf, yf, zf-1), grad(P[BA+1], xf-1, yf, zf-1), u), 
        lerp(grad(P[AB+1], xf, yf-1, zf-1), grad(P[BB+1], xf-1, yf-1, zf-1), u), 
        v), 
      w);
}

double Perlin::turbulence(double x, double y, double z, unsigned octaves, double freq, double gain)
{
  double t = 0;
  double scale = 1;
  double amplitude = 1;
  for(unsigned i = 0; i < octaves; i++)
  {
    t += amplitude * Perlin::noise(x * scale, y * scale, z * scale);
    scale *= freq;
    amplitude *= gain;
  }

  return std::abs(t);
};

double Perlin::marble(double x, double y, double z)
{
  // http://www.sci.utah.edu/~leenak/IndStudy_reportfall/MarbleCode.txt 
  return cos(z * 0.1 + 6 * turbulence(x, y, z, 5, 2, 0.6));
}

double Perlin::wood(double x, double y, double z)
{
  double grain = ((1.0 + Perlin::noise(std::abs(x), std::abs(y), std::abs(z))) / 2.0) * 30;
  return (grain - (int)grain);
}

double Perlin::cloud(double x, double y, double z)
{
  return cos(z * 0.5 + 2 * turbulence(x, y, z, 6, 2, 0.5) * 0.9);
}

