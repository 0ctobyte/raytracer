#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "scene.hpp"
#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
class Light : Primitive {
public:
  Light();
  Light(const Colour& col, const Point3D& pos, double* foff)
    : colour(col)
    , position(pos)
  {
    falloff[0] = foff[0];
    falloff[1] = foff[1];
    falloff[2] = foff[2];
  }
  ~Light();
  
  Colour getColour() const
  {
    return colour;
  }
  Colour& getColour()
  {
    return colour;
  }

  virtual Point3D getPosition() const
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

  virtual bool isPointLight()
  {
    return true;
  }

  virtual bool intersect(const Ray& ray, Intersection& j) const;

  friend std::ostream& operator<<(std::ostream& out, const Light& l);

protected:
  Colour colour;
  Point3D position;
  double falloff[3];
};

class DiscLight : Light {
public:
  DiscLight(const Colour& col, const Point3D& pos, double* foff, double radius, const Vector3D* basis)
    : Light(col, pos, foff)
    , m_radius(radius)
  {
    m_basis[0] = basis[0].normalized();
    m_basis[1] = basis[1].normalized();
    m_normal = basis[0].cross(basis[1]).normalized();
  }
  ~DiscLight();

  virtual bool isPointLight()
  {
    return false;
  }

  virtual Point3D getPosition() const; 
  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  double m_radius;
  Vector3D m_basis[2];
  Vector3D m_normal;
};

#endif
