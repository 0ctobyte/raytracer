#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <utility>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  typedef std::vector<int> Face;
  
  Mesh(const std::vector<Point3D>& verts, const std::vector<Face>& faces);

  virtual bool intersect(const Ray& ray, Intersection& j) const;
  
protected:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  NonhierSphere m_boundingBall;

  NonhierSphere getBoundingBall(const std::vector<Point3D>& verts) const;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

// A polygonal mesh with triangular faces and vertex normals
class TriMesh : public Mesh {
public:
  typedef std::vector<std::tuple<int, int>> TriFace;

  TriMesh(const std::vector<Point3D>& verts, const std::vector<Vector3D>& normals, const std::vector<TriFace>& tfaces);
  TriMesh(const std::vector<Point3D>& verts, const std::vector<Face>& faces); 
  
  virtual bool intersect(const Ray& ray, Intersection& j) const;

protected:
  std::vector<Vector3D> m_normals;
  std::vector<TriFace> m_tfaces;

  std::vector<TriFace> triangulate(const std::vector<Face>& faces);
  std::vector<Vector3D> normalate(const std::vector<Point3D>& verts, const std::vector<Face>& faces);
};

#endif
