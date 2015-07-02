#include "light.hpp"
#include <iostream>
#include <limits>
#include <cmath>
#include <cstdlib>

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

Light::~Light()
{
}

bool Light::intersect(const Ray& ray, Intersection& j) const
{
  Vector3D u = (position - ray.origin()).normalized();
  Vector3D v = ((ray.origin() + 1.0*ray.direction()) - ray.origin()).normalized();

  if(fabs(u.dot(v) - 1.0) > std::numeric_limits<double>::epsilon()) return false;

  j.q = position;
  j.n = -u;
  j.isLight = true;
  j.lightColour = colour;

  return true;
}

DiscLight::~DiscLight()
{
}

bool DiscLight::intersect(const Ray& ray, Intersection& j) const
{
  // First we intersect with the plane containing the disc
  double den = m_normal.dot(ray.direction());
  if(fabs(den) < std::numeric_limits<double>::epsilon()) return false;

  // The intersection point cannot be behind the ray
  double t = m_normal.dot(position - ray.origin()) * (1 / den);
  if(t < 0) return false;

  // Now get the intersection point
  Point3D Q = ray.origin() + t * ray.direction();

  // Check if the intersection point is within the radius of the disc
  if((Q-position).dot(Q-position) > (m_radius*m_radius)) return false;

  // Flip the normal if the backface is facing the ray
  j.q = Q;
  j.n = ((ray.origin() - position).dot(m_normal) < 0) ? -m_normal : m_normal;
  j.isLight = true;
  j.lightColour = colour;

  return true;
}

Point3D DiscLight::getPosition() const
{
  srand(time(NULL));
  double e1 = rand() / RAND_MAX;
  double e2 = rand() / RAND_MAX;

  return Matrix4x4().rotate(e2*360.0, m_normal) * (position + (e1 * m_radius) * m_perp);
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << l.colour << ", " << l.position << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
