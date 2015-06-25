#include "mesh.hpp"
#include <iostream>
#include <cmath>
#include <limits>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts)
  , m_faces(faces)
  , m_boundingBall(getBoundingBall(verts))
{
}

NonhierSphere Mesh::getBoundingBall(const std::vector<Point3D>& verts) const
{
  Point3D C = verts[0] + 0.5*(verts[1]-verts[0]);
  double radius = (verts[1] - verts[0]).length() / 2.0;

  // This is the bouncing ball algorithm. Take any two points and construct the sphere's center
  // at the center point of the line between the two vertices and raidus half the distance
  // between the two vertices
  // Then loop through each vertex. If it is outside the sphere then move the sphere's center
  // a little bit towards the point while growing the radius as well so that the point is now inside 
  // the sphere. Keep doing this until there are no vertices outside the sphere
  for(bool outsider = true; outsider == true;)
  {
    outsider = false;
    for(auto v : verts)
    {
      double length = (v-C).length();
      if(length > radius)
      {
        double diff = length - radius;
        double delta = diff / length;
        C = C + (0.5 * delta)*(v-C);
        radius = radius + (0.5 * diff);
        outsider = true;
      }
    }
  }

  return NonhierSphere(C, radius);
}

bool Mesh::intersect(const Ray& ray, Intersection& j) const
{
  bool intersected = false;

  // Check if bounding ball has been intersected first
  // If not then the mesh cannot have been intersected
  Intersection k;
  bool bball_intersected = m_boundingBall.intersect(ray, k);
  if(bball_intersected)
  {
    // Loop through each face and check if there is an intersection
    double prev_t = std::numeric_limits<double>::infinity();
    for(auto face : m_faces)
    {
      // Compute the normal for the face
      Point3D P0 = m_verts[face[0]];
      Point3D P1 = m_verts[face[1]];
      Point3D P2 = m_verts[face[2]];

      Vector3D n = (P1-P0).cross(P2-P0).normalized();

      // Now check if the ray intersects the polygon containing the face
      // If denom is 0 then the ray does not intersect the plane at all
      double denom = n.dot(ray.direction());
      if(fabs(denom) < std::numeric_limits<double>::epsilon()) continue;

      // If t is negative or a previous intersection has a smaller t (meaning it is closer to the
      // ray's origin) then disregard this face and continue
      double t = n.dot(P0 - ray.origin()) / denom;
      if(t < 0 || prev_t < t) continue;

      // Calculate intersection point
      Point3D Q = ray.origin() + t*ray.direction();

      bool outside = false;
      for(size_t i = 0; i < face.size(); i++)
      {
        Point3D Q0 = (i == 0) ? m_verts[face.back()] : m_verts[face[i-1]];
        Point3D Q1 = m_verts[face[i]];

        if((Q1-Q0).cross(Q-Q0).dot(n) < 0)
        {
          outside = true;
          break;
        }
      }

      if(!outside)
      {
        // It is within the bounds of the polygon
        intersected = true;
        prev_t = t;
        j.q = Q;
        j.n = n;
      }
    }
  }

  return intersected;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
