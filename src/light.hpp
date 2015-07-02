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
  DiscLight(const Colour& col, const Point3D& pos, double* foff, double radius, const Vector3D& normal)
    : Light(col, pos, foff)
    , m_radius(radius)
    , m_normal(normal.normalized())
  {
    // Find a perpendicular vector to the normal on the plane
    // This technique is described by Hughes and Moller in their paper:
    // “Building an Orthonormal Basis from a Unit Vector”
    if(m_normal[2] <= m_normal[0] && m_normal[2] <= m_normal[1])
    {
      m_perp = Vector3D(-m_normal[1], m_normal[0], 0.0);
    }
    else if(m_normal[1] <= m_normal[0])
    {
      m_perp = Vector3D(-m_normal[2], 0.0, m_normal[0]);
    }
    else
    {
      m_perp = Vector3D(0.0, -m_normal[2], m_normal[1]);
    }

    m_perp.normalize();
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
  Vector3D m_normal;
  Vector3D m_perp;
};

#endif
