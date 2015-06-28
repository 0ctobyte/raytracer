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
    double epsilon = std::numeric_limits<double>::epsilon();
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
      if(fabs(denom) < epsilon) continue;

      // If t is negative or a previous intersection has a smaller t (meaning it is closer to the
      // ray's origin) then disregard this face and continue
      double t = n.dot(P0 - ray.origin()) / denom;
      if(t < 0 || prev_t < t) continue;

      // Calculate intersection point
      Point3D Q = ray.origin() + t*ray.direction();

      // It makes it easier to check if the intersection point is inside the face when both
      // the face and the intersection point are projected on to the 2D plane. The 2D plane
      // shall be the plane corresponding to dropping the largest coordinate of the normal vector
      // This prevents the polygon being projected into a line on the 2D plane
      int i1 = 0, i2 = 0;
      if(fabs(n[2]) > fabs(n[0]) && fabs(n[2]) > fabs(n[1]))
      {
        i1 = 0;
        i2 = 1;
      }
      else if(fabs(n[1]) > fabs(n[0]))
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
      for(size_t j = 0; j < face.size(); j++)
      {
        Point3D E0 = (j == 0) ? Point3D(m_verts[face.back()][i1]-Q[i1], m_verts[face.back()][i2]-Q[i2], 0.0) : Point3D(m_verts[face[j-1]][i1]-Q[i1], m_verts[face[j-1]][i2]-Q[i2], 0.0);
        Point3D E1 = Point3D(m_verts[face[j]][i1]-Q[i1], m_verts[face[j]][i2]-Q[i2], 0.0);

        // 1 if positive, 0 otherwise
        int sign0 = (E0[1] >= 0) ? 1 : 0;
        int sign1 = (E1[1] >= 0) ? 1 : 0;

        // If both of the v coordinates have the same sign, then the edge definitely doesn't cross the positive u axis
        if(sign0 == sign1) continue;

        // If both the u coordinates is leq to 0, then the edge definitely doesn't cross the ray on the positive u axis
        if(E0[0] <= 0 && E1[0] <= 0) continue;

        // If both the u coordinates are greather than 0, then the edge definitely crosses the positive u axis
        if(E0[0] > epsilon && E1[0] > epsilon) {
          edge_crossings++;
        }
        else
        {
          // Otherwise the edge may or may not cross the positive u axis. We must calculate the intersection point
          // We know that the v coordinate of the intersection point must be 0. We just have to check the u coordinate
          // and see if it is greater than 0
          double s = (-E0[1]) / (E1[1] - E0[1]);
          double u = E0[0] + s * (E1[0] - E0[0]);
          if(u > epsilon) edge_crossings++;
        }
      }


      if(edge_crossings & 0x1)
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
