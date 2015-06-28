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

  double epsilon = std::numeric_limits<double>::epsilon();
  double prev_t = std::numeric_limits<double>::infinity();
  bool intersection = false;
  for(int i = 0; i < 6; i++)
  {
    // Calculate the normal vector for the plane containing this face
    Vector3D fn = (fp[i][2]-fp[i][0]).cross(fp[i][1]-fp[i][0]).normalized();

    // If this is 0, then the ray is parallel or lies completely within the plane. Reject it
    double den = fn.dot(ray.direction());
    if(fabs(den) < epsilon) continue;

    // Calculate the intersection parameter t
    double t = (fp[i][0] - ray.origin()).dot(fn) / den;

    // If t < 0 then the intersection point is behind the ray's origin. Reject it
    // Reject it if it is also further away then a previous intersection with another face
    if(t < 0 || prev_t < t) continue;

    Point3D Q = ray.origin() + t*ray.direction();

    // It makes it easier to check if the intersection point is inside the face when both
    // the face and the intersection point are projected on to the 2D plane. The 2D plane
    // shall be the plane corresponding to dropping the largest coordinate of the normal vector
    // This prevents the polygon being projected into a line on the 2D plane
    int i1 = 0, i2 = 0;
    if(fabs(fn[2]) > fabs(fn[0]) && fabs(fn[2]) > fabs(fn[1]))
    {
      i1 = 0;
      i2 = 1;
    }
    else if(fabs(fn[1]) > fabs(fn[0]))
    {
      i1 = 0;
      i2 = 2;
    }
    else
    {
      i1 = 1;
      i2 = 2;
    }

    // Now for each edge, project the points to the 2D plane and translate the points such that the intersection point
    // is centered on the origin. We then shoot a "ray" in the positive u (or x) axis and count the number of times
    // this ray intersects with an edge. If even, then the intersection point is outside the face, otherwise it is inside
    int edge_crossings = 0;
    for(int j = 0; j < 4; j++)
    {
      Point3D P0 = (j == 0) ? Point3D(fp[i][3][i1]-Q[i1], fp[i][3][i2]-Q[i2], 0.0) : Point3D(fp[i][j-1][i1]-Q[i1], fp[i][j-1][i2]-Q[i2], 0.0);
      Point3D P1 = Point3D(fp[i][j][i1]-Q[i1], fp[i][j][i2]-Q[i2], 0.0);

      // 1 if positive, 0 otherwise
      int sign0 = (P0[1] >= 0) ? 1 : 0;
      int sign1 = (P1[1] >= 0) ? 1 : 0;

      // If both of the v coordinates have the same sign, then the edge definitely doesn't cross the positive u axis
      if(sign0 == sign1) continue;

      // If both the u coordinates is leq to 0, then the edge definitely doesn't cross the ray on the positive u axis
      if(P0[0] <= 0 && P1[0] <= 0) continue;

      // If both the u coordinates are greather than 0, then the edge definitely crosses the positive u axis
      if(P0[0] > epsilon && P1[0] > epsilon) {
        edge_crossings++;
      }
      else
      {
        // Otherwise the edge may or may not cross the positive u axis. We must calculate the intersection point
        // We know that the v coordinate of the intersection point must be 0. We just have to check the u coordinate
        // and see if it is greater than 0
        double s = (-P0[1]) / (P1[1] - P0[1]);
        double u = P0[0] + s * (P1[0] - P0[0]);
        if(u > epsilon) edge_crossings++;
      }
    }

    // Check if the number of edge crossings is even
    if(edge_crossings & 0x1)
    {
      intersection = true;
      prev_t = t;
      j.q = Q;
      j.n = fn;
    }
  }

  return intersection;
}

