#include "primitive.hpp"
#include "polyroots.hpp"

#include <cmath>
#include <algorithm>
#include <limits>

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray& ray, Intersection& j) const
{
  NonhierSphere sphere(Point3D(0.0, 0.0, 0.0), 1.0);
  return sphere.intersect(ray, j);
}

Cube::~Cube()
{
}

bool Cube::intersect(const Ray& ray, Intersection& j) const
{
  NonhierBox box(Point3D(0.0, 0.0, 0.0), 1.0);
  return box.intersect(ray, j);
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(const Ray& ray, Intersection& j) const
{
  // Ray/sphere intersection test
  // Equation for a sphere centered at p_c with radius r and arbitrary point on the sphere p:
  // (p - p_c) . (p - p_c) - r^2 = 0;
  // Equation for the line coincident with a ray at origin p_o and direction d:
  // t*d + p_o = p
  // Substituting the equation for the ray into the equation for the sphere and solving for parameter t
  // will give use the intersection point if one exists
  // (t*d + p_o - p_c) . (t*d + p_o - p_c) - r^2 = 0
  // (d . d)*t^2 + (2d . (p_o - p_c))*t - ((p_o - p_c) . (p_o - p_c)) - r^2 = 0
  // A = (d . d)
  // B = (2d . (p_o - p_c))
  // C = ((p_o - p_c) . (p_o - p_c)) - r^2
  
  double roots[2];

  Vector3D v = ray.origin() - m_pos;
  double A = ray.direction().dot(ray.direction());
  double B = (2*ray.direction()).dot(v);
  double C = v.dot(v) - m_radius*m_radius;

  size_t num_roots = quadraticRoots(A, B, C, roots);

  // If there are no roots==no intersections
  // If there is one root, then the ray intersect the sphere on a tangent
  // If there is two roots, then one is the intersection with the sphere from the front as the ray enters the sphere
  // and the other is the intersection through the back as it leaves the sphere
  // For this case we only take the smallest t value since that is the point where the ray enters the sphere which
  // will be the visible point of the sphere. Of course t must not be negative otherwise it is behind the eye point
  if(num_roots > 0)
  {
    // If the ray orginates inside the sphere, then we need to get the max of both roots
    // Otherwise we use the min. If there is only one root, then just use that of course
    // If t is still less than 0 than the sphere is completely behind the ray's origin
    double min = std::min<double>(roots[0], roots[1]);
    double t = (num_roots == 1) ? roots[0] : ((min < 0) ? std::max<double>(roots[0], roots[1]) : min);
    if(t < 0) return false;
    j.q = ray.origin() + t*ray.direction();
    j.n = (j.q - m_pos).normalized();
    return true;
  }
  
  return false;
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(const Ray& ray, Intersection& j) const
{
  Vector3D r_dir(1.0 / ray.direction()[0], 1.0 / ray.direction()[1], 1.0 / ray.direction()[2]);
  Point3D bmin(m_pos);
  Point3D bmax(m_pos[0]+m_size, m_pos[1]+m_size, m_pos[2]+m_size);

  // Get the intersection interval for the near and far x planes
  double tmin = (bmin[0] - ray.origin()[0]) * r_dir[0];
  double tmax = (bmax[0] - ray.origin()[0]) * r_dir[0];
  Vector3D nmin(-1.0, 0.0, 0.0);
  Vector3D nmax(1.0, 0.0, 0.0);

  // Swap if txmax is closer than txmin
  if(tmax < tmin)
  {
    std::swap<double>(tmin, tmax);
    nmin = Vector3D(1.0, 0.0, 0.0);
    nmax = Vector3D(-1.0, 0.0, 0.0);
  }

  // Get the intersection interval for the near and far y planes
  double tymin = (bmin[1] - ray.origin()[1]) * r_dir[1];
  double tymax = (bmax[1] - ray.origin()[1]) * r_dir[1];
  Vector3D nymin(0.0, -1.0, 0.0);
  Vector3D nymax(0.0, 1.0, 0.0);

  // Swap if tymax is closer than tymin
  if(tymax < tymin) 
  {
    std::swap<double>(tymin, tymax);
    nymin = Vector3D(0.0, 1.0, 0.0);
    nymax = Vector3D(0.0, -1.0, 0.0);
  }

  // Now if the intervals don't overlap then the ray does not intersect at all
  if(tmax < tymin || tymax < tmin) return false;

  // Now tmin is the max of tmin and tymin (i.e. the interval is closer to the true intersection point)
  if(tymin > tmin)
  {
    tmin = tymin;
    nmin = nymin; 
  }

  // tmax is the min of tmax and tymax
  if(tymax < tmax)
  {
    tmax = tymax;
    nmax = nymax;
  }

  // Get the intersection interval for the near and far z planes
  double tzmin = (bmin[2] - ray.origin()[2]) * r_dir[2];
  double tzmax = (bmax[2] - ray.origin()[2]) * r_dir[2];
  Vector3D nzmin(0.0, 0.0, -1.0);
  Vector3D nzmax(0.0, 0.0, 1.0);

  // Swap if tzmax is closer than tzmin
  if(tzmax < tzmin)
  {
    std::swap<double>(tzmin, tzmax);
    nzmin = Vector3D(0.0, 0.0, 1.0);
    nzmax = Vector3D(0.0, 0.0, -1.0);
  }

  // If the intervals don't overlap, well the ray doesn't intersect at all
  if(tmax < tzmin || tzmax < tmin) return false;

  // Take the max of the two min intersection intervals
  if(tzmin > tmin)
  {
    tmin = tzmin;
    nmin = nzmin;
  }

  // Take the min of the two max intersection intervals
  if(tzmax < tmax)
  {
    tmax = tzmax;
    nmax = nzmax;
  }

  // Check if both intersection distances are less than 0. The intersection point is then behind the ray's origin
  // If only tmin is less than 0, then the ray originates from within the box and we take tmax as the intersection interval
  double t = tmin;
  Vector3D n = nmin;
  if(tmin < 0)
  {
    t = tmax;
    n = nmax;
  }
  
  if(t < 0) return false;

  j.q = ray.origin() + t * ray.direction();
  j.n = n;

  return true;
}

