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

Cone::~Cone()
{
}

bool Cone::intersect(const Ray& ray, Intersection& j) const
{
  NonhierCone cone(Point3D(0.0, 0.0, 0.0), 1.0);
  return cone.intersect(ray, j);
}

Cylinder::~Cylinder()
{
}

bool Cylinder::intersect(const Ray& ray, Intersection& j) const
{
  NonhierCylinder cylinder(Point3D(0.0, 0.0, 0.0), 1.0, 1.0);
  return cylinder.intersect(ray, j);
}

Cube::~Cube()
{
}

bool Cube::intersect(const Ray& ray, Intersection& j) const
{
  NonhierBox box(Point3D(0.0, 0.0, 0.0), 1.0);
  return box.intersect(ray, j);
}

Plane::~Plane()
{
}

bool Plane::intersect(const Ray& ray, Intersection& j) const
{
  NonhierPlane plane(Point3D(0.0, 0.0, 0.0), 1.0);
  return plane.intersect(ray, j);
}

Torus::~Torus()
{
}

bool Torus::intersect(const Ray& ray, Intersection& j) const
{
  NonhierTorus torus(Point3D(0.0, 0.0, 0.0), 1, 0.5);
  return torus.intersect(ray, j);
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

NonhierCone::~NonhierCone()
{
}

bool NonhierCone::intersect(const Ray& ray, Intersection& j) const
{
  // The implicit equation of an infinite double cone: x^2 + y^2 = z^2
  // Need to solve the quadratic formula to get t
  Vector3D O = ray.origin() - m_pos;
  Vector3D d = ray.direction();

  double A = d[0]*d[0] + d[1]*d[1] - d[2]*d[2];
  double B = 2*O[0]*d[0] + 2*O[1]*d[1] - 2*O[2]*d[2];
  double C = O[0]*O[0] + O[1]*O[1] - O[2]*O[2];

  double zmax = 0.0;
  double zmin = -m_height;

  double roots[2];

  size_t num_roots = quadraticRoots(A, B, C, roots);

  if(num_roots > 0)
  {
    double t1 = roots[0];
    double t2 = (num_roots == 2) ? roots[1] : std::numeric_limits<double>::infinity();

    // Make sure the intersection interval isn't negative
    if(t1 > 0 || t2 > 0)
    {
      // Calculate the near and far z coordinates
      double z1 = O[2] + t1*d[2];
      double z2 = O[2] + t2*d[2];

      // Check to see if the intersection times are within the bounds of the finite open-ended cylinder
      // Find the smallest non-negative intersection time 
      double t = (z1 < zmax && z1 > zmin && t1 > 0) ? t1 : std::numeric_limits<double>::infinity();
      t = (z2 < zmax && z2 > zmin && t2 > 0) ? ((t2 < t) ? t2 : t) : t;

      // Found a valid intersection!
      if(!std::isinf<double>(t))
      {
        
        // Calculate the intersection interval with the end cap
        double t3 = (zmin - O[2]) / d[2];

        // The end cap may have been intersected only if there is only one root or the z1 and z2 are on either side of zmin
        // If so, we check if the intersection point is closer than any other intersection points
        Point3D Q = ray.origin() + t*ray.direction();
        Vector3D N(2*Q[0], 2*Q[1], -2*Q[2]);
     
        int signz1 = ((z1-zmin) > 0) ? 1 : 0;
        int signz2 = ((z2-zmin) > 0) ? 1 : 0;
        if(signz1 != signz2 && t3 > 0 && t3 < t)
        {
          t = t3;
          Q = ray.origin() + t*ray.direction();
          N = Vector3D(0.0, 0.0, zmin);
        }

        // To find the normal we just take the gradient and plug the coordinate values for the intersection point into the gradient result
        j.q = Q;
        j.n = N.normalized();

        return true;
      }
    }
  }

  return false;
}

NonhierCylinder::~NonhierCylinder()
{
}

bool NonhierCylinder::intersect(const Ray& ray, Intersection& j) const
{
  // Ray/cylinder intersection test
  // The infinite cylinder aligned along the z axis has the implicit formula: (x - p_c.x)^2 + (y - p_c.y)^2 = r^2
  // Equation of ray: p_o + t*d = p
  // Substituting the ray equation in the cylinder formula:
  // ((p_o.x-p_c.x) + t*d.x)^2 + ((p_o.y-p_c.y) + t*d.y)^2 = r^2
  // -> (p_o.x-p_c.x)^2 + 2t*d.x*(p_o.x-p_c.x) + t^2*d.x^2 + (p_o.y-p_c.y)^2 + 2t*d.y*(p_o.y-p_c.y) + t^2*d.y^2 - r^2 = 0
  // -> (d.x^2+d.y^2)*t^2 + (2*d.x*(p_o.x-p_c.x)+2*d.y*(p_o.y-p_c.y))*t + ((p_o.x-p_c.x)^2+(p_o.y-p_c.y)^2-r^2) = 0
  // Thus: A = (d.x^2+d.y^2), B = (2*d.x*(p_o.x-p_c.x)+2*d.y*(p_o.y-p_c.y)), C = ((p_o.x-p_c.x)^2+(p_o.y-p_c.y)^2-r^2)
  // We must solve this quadratic equation to get the value of t (intersection time)
  double roots[2];

  Vector3D v = ray.origin() - m_pos;
  double zmax = m_height / 2.0;
  double zmin = -zmax;
  double A = ray.direction()[0]*ray.direction()[0] + ray.direction()[1]*ray.direction()[1];
  double B = 2*(ray.direction()[0]*v[0] + ray.direction()[1]*v[1]);
  double C = v[0]*v[0] + v[1]*v[1] - m_radius*m_radius;

  size_t num_roots = quadraticRoots(A, B, C, roots);

  // If there are no roots then there are no intersections
  // If there is one root, then there is a tangential intersection
  // If there are two roots, then the ray passes through the cylinder
  // For this case we take the smallest root greater than 0 to find the nearest intersection point
  // If the roots are negative then the cylinder is behind the ray
  double t = std::numeric_limits<double>::infinity();
  Vector3D N;
  bool intersected = false;
  if(num_roots > 0)
  {
    double t1 = roots[0];
    double t2 = (num_roots == 1) ? std::numeric_limits<double>::infinity() : roots[1];

    // The intersection times are negative which means the cylinder is completely behind the ray
    if(t1 > 0 || t2 > 0)
    {
      // Calculate the z coordinates of the intersection times
      double z1 = v[2] + t1*ray.direction()[2];
      double z2 = v[2] + t2*ray.direction()[2];

      // Check to see if the intersection times are within the bounds of the finite open-ended cylinder
      // Find the smallest non-negative intersection time 
      t = (z1 < zmax && z1 > zmin && t1 > 0) ? t1 : t;
      t = (z2 < zmax && z2 > zmin && t2 > 0) ? ((t2 < t) ? t2 : t) : t;

      // Found a valid intersection
      if(!std::isinf<double>(t)) 
      {
        // The normal is essentially the vector from the center point to the intersection point removing the component
        // corresponding to the axis which the cylinder is aligned (the Z axis in this case)
        N = (ray.origin() + t*ray.direction())-m_pos;
        N[2] = 0.0;

        intersected = true;
      }
    }
  }

  // We must test for intersection with the endcaps regardless whether the ray intersects the body of the cylinder
  Vector3D nzmin(0.0, 0.0, -1.0);
  double denom = nzmin.dot(ray.direction());
  if(fabs(denom) > std::numeric_limits<double>::epsilon())
  {
    // We first checked if the ray intersects with the plane containing the end cap (disc)
    // Then we check if the intersection point is within the disc (P-Q).(P-Q) <= r^2
    Point3D Q(0.0, 0.0, zmin);
    double tzmin = nzmin.dot(Q-v) / denom;
    Point3D P = Point3D(v[0], v[1], v[2]) + tzmin*ray.direction();
    if(tzmin < t && tzmin > 0 && ((P-Q).dot(P-Q) - m_radius*m_radius) < std::numeric_limits<double>::epsilon())
    {
      t = tzmin;
      N = nzmin;
      intersected = true;
    }
  }

  // Do the same check for the other end cap
  Vector3D nzmax(0.0, 0.0, 1.0);
  denom = nzmax.dot(ray.direction());
  if(fabs(denom) > std::numeric_limits<double>::epsilon())
  {
    Point3D Q(0.0, 0.0, zmax);
    double tzmax = nzmax.dot(Q-v) / denom;
    Point3D P = Point3D(v[0], v[1], v[2]) + tzmax*ray.direction();
    if(tzmax < t && tzmax > 0 && ((P-Q).dot(P-Q) - m_radius*m_radius) < std::numeric_limits<double>::epsilon())
    {
      t = tzmax;
      N = nzmax;
      intersected = true;
    }
  }

  if(intersected)
  {
    j.q = ray.origin() + t*ray.direction();
    j.n = N.normalized();
  }

  return intersected;
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

NonhierPlane::~NonhierPlane()
{
}

bool NonhierPlane::intersect(const Ray& ray, Intersection& j) const
{
  Vector3D normal(0.0, 1.0, 0.0);

  // Simple intersection with infinite plane calculation. If the denominator is 0, then the ray does not intersect at all
  double denom = normal.dot(ray.direction());
  if(fabs(denom) < std::numeric_limits<double>::epsilon()) return false;

  double t = normal.dot(m_pos-ray.origin()) / denom;
  if(t < 0) return false;

  Point3D P = ray.origin() + t*ray.direction();

  // Now we make sure that the intersection point lies within the truncated plane
  if(P[0] < (m_pos[0]-m_size) || P[0] > (m_pos[0]+m_size)) return false;
  if(P[2] < (m_pos[2]-m_size) || P[2] > (m_pos[2]+m_size)) return false;

  j.q = P;
  j.n = normal;

  return true;
}

NonhierTorus::~NonhierTorus()
{
}

bool NonhierTorus::intersect(const Ray& ray, Intersection& j) const
{
  // The implicit formula for the surface of a torus centered at c and lying on the xy plane: (x^2 + y^2 + z^2 + R^2 - r^2)^2 + 4R^2(z^2 - r^2)  
  // The parametric equation of a ray: O + t*d = P
  // I'm not going to go through the process of solving this equation. Instead I got the final formula here:
  // http://www.emeyex.com/site/projects/raytorus.pdf
  Vector3D p = ray.origin() - m_pos;
  Vector3D d = ray.direction();

  double R2 = m_oradius*m_oradius;
  double r2 = m_iradius*m_iradius;
  double a = d.dot(d);
  double b = 2*(p.dot(d));
  double y = p.dot(p) - r2 - R2;

  double _A = 1.0 / (a*a);
  double B = 2*a*b;
  double C = (b*b) + 2*a*y + 4*R2*(d[2]*d[2]);
  double D = 2*b*y + 8*R2*p[2]*d[2];
  double E = (y*y) + 4*R2*(p[2]*p[2]) - 4*R2*r2;

  double roots[4];

  // Now solve for the roots which shall give us values for t
  size_t num_roots = quarticRoots(B * _A, C * _A, D * _A, E * _A, roots);

  // If there are no roots, then no intersection
  if(num_roots == 0) return false;


  // If all the roots are less than 0, then the whole torus is behind the ray
  // Now we want the find the smallest intersection point greater than 0
  double t = std::numeric_limits<double>::infinity(); 
  for(size_t i = 0; i < num_roots; i++)
  {
    t = (roots[i] > 0 && roots[i] < t) ? roots[i] : t;
  }

  // No intersection points greater than 0 means the whole torus is behind the ray
  if(std::isinf(t)) return false;

  // To find the surface normal, we take the partial derivative of the implicit formula of the torus
  // with respect to each of the coordinates and then plug in the coordinate values from the intersection point
  Point3D Q = ray.origin() + t*ray.direction();
  double qx2 = Q[0]*Q[0];
  double qy2 = Q[1]*Q[1];
  double qz2 = Q[2]*Q[2];
  double nx = 4*Q[0]*(qx2 + qy2 + qz2 - r2 - R2);
  double ny = 4*Q[1]*(qx2 + qy2 + qz2 - r2 - R2);
  double nz = 4*Q[2]*(qz2 + qy2 + qz2 - r2 - R2) + 8*R2*Q[2];
  
  j.q = Q;
  j.n = Vector3D(nx, ny, nz).normalized();

  return true;
}

