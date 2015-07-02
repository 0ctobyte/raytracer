#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
class Light {
public:
  Light();
  
  Colour getColour() const
  {
    return colour;
  }
  Colour& getColour()
  {
    return colour;
  }

  Point3D getPosition() const
  {
    return position;
  }
  Point3D& getPosition()
  {
    return position;
  }

  double getAttenuation(double r) const
  {
    return (1.0 / (falloff[0] + falloff[1]*r + falloff[2]*(r*r)));
  }
  double* getFalloff()
  {
    return falloff;
  }


  friend std::ostream& operator<<(std::ostream& out, const Light& l);
private:
  Colour colour;
  Point3D position;
  double falloff[3];
};


#endif
