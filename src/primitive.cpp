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
  // This algorithm is kinda inefficient but it fucking works so whatevs
  // First gather the points and normals for all faces
  double x = m_pos[0], y = m_pos[1], z = m_pos[2], r = m_size;
  Point3D fp[6][4] = {
    {Point3D(x, y, z), Point3D(x+r, y, z), Point3D(x+r, y+r, z), Point3D(x, y+r, z)},
    {Point3D(x, y+r, z), Point3D(x+r, y+r, z), Point3D(x+r, y+r, z+r), Point3D(x, y+r, z+r)},
    {Point3D(x+r, y, z), Point3D(x+r, y, z+r), Point3D(x+r, y+r, z+r), Point3D(x+r, y+r, z)},
    {Point3D(x, y, z), Point3D(x, y+r, z), Point3D(x, y+r, z+r), Point3D(x, y, z+r)},
    {Point3D(x, y, z+r), Point3D(x, y+r, z+r), Point3D(x+r, y+r, z+r), Point3D(x+r, y, z+r)},
    {Point3D(x, y, z), Point3D(x, y, z+r), Point3D(x+r, y, z+r), Point3D(x+r, y, z)}
  };
  Vector3D fn[6] = {
    (fp[0][2]-fp[0][0]).cross(fp[0][1]-fp[0][0]).normalized(),
    (fp[1][2]-fp[1][0]).cross(fp[1][1]-fp[1][0]).normalized(),
    (fp[2][2]-fp[2][0]).cross(fp[2][1]-fp[2][0]).normalized(),
    (fp[3][2]-fp[3][0]).cross(fp[3][1]-fp[3][0]).normalized(),
    (fp[4][2]-fp[4][0]).cross(fp[4][1]-fp[4][0]).normalized(),
    (fp[5][2]-fp[5][0]).cross(fp[5][1]-fp[5][0]).normalized(),
  };

  // This works similar to polygon intersection where we find the parameter t
  // which gives the intersection point of the ray with the plane containing the
  // face. We then check if the point is "inside" each of the edges of the face
  double prev_t = std::numeric_limits<double>::infinity();
  bool intersection = false;
  for(int i = 0; i < 6; i++)
  {
    double den = fn[i].dot(ray.direction());
    if(fabs(den) < std::numeric_limits<double>::epsilon()) continue;

    double t = (fp[i][0] - ray.origin()).dot(fn[i]) / den;
    if(t < 0) continue;

    if(prev_t < t) continue;

    Point3D Q = ray.origin() + t*ray.direction();

    // The cross product of the two vectors should give a vector that is in a similar direction to
    // the normal of the face
    if((fp[i][0]-fp[i][1]).cross(Q-fp[i][1]).dot(fn[i]) < 0) continue;
    if((fp[i][1]-fp[i][2]).cross(Q-fp[i][2]).dot(fn[i]) < 0) continue;
    if((fp[i][2]-fp[i][3]).cross(Q-fp[i][3]).dot(fn[i]) < 0) continue;
    if((fp[i][3]-fp[i][0]).cross(Q-fp[i][0]).dot(fn[i]) < 0) continue;

    prev_t = t;
    intersection = true;
    j.q = Q;
    j.n = fn[i];
  }

  return intersection;
}

