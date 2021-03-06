#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <memory>
#include "algebra.hpp"
#include "primitive.hpp"
#include "mesh.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(std::shared_ptr<SceneNode> child)
  {
    m_children.push_back(child);
  }

  void remove_child(std::shared_ptr<SceneNode> child)
  {
    m_children.remove(child);
  }

  virtual bool intersect(const Ray& ray, Intersection& i) const;

  virtual void flatten();

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<std::shared_ptr<SceneNode>> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name, std::shared_ptr<Primitive> primitive);
  virtual ~GeometryNode();

  virtual bool intersect(const Ray& ray, Intersection& i) const;

  std::shared_ptr<const Material> get_material()
  {
    return m_material;
  }
  std::shared_ptr<Material> get_material() const 
  {
    return m_material;
  }

  void set_material(std::shared_ptr<Material> material)
  {
    m_material = nullptr;
    m_material = material;
  }

protected:
  std::shared_ptr<Material> m_material;
  std::shared_ptr<Primitive> m_primitive;
};

class ConstructiveSolidGeometryNode : public GeometryNode {
public:
  ConstructiveSolidGeometryNode(const std::string& name, std::shared_ptr<GeometryNode> A, std::shared_ptr<GeometryNode> B);
  virtual ~ConstructiveSolidGeometryNode();

  virtual bool intersect(const Ray& ray, Intersection& i) const = 0;

protected:
  std::shared_ptr<GeometryNode> m_A;
  std::shared_ptr<GeometryNode> m_B;
};

class UnionNode : public ConstructiveSolidGeometryNode {
public:
  UnionNode(const std::string& name, std::shared_ptr<GeometryNode> A, std::shared_ptr<GeometryNode> B);
  virtual ~UnionNode();

  virtual bool intersect(const Ray& ray, Intersection& i) const;
};

class IntersectionNode : public ConstructiveSolidGeometryNode {
public:
  IntersectionNode(const std::string& name, std::shared_ptr<GeometryNode> A, std::shared_ptr<GeometryNode> B);
  virtual ~IntersectionNode();

  virtual bool intersect(const Ray& ray, Intersection& i) const;
};

class DifferenceNode : public ConstructiveSolidGeometryNode {
public:
  DifferenceNode(const std::string& name, std::shared_ptr<GeometryNode> A, std::shared_ptr<GeometryNode> B);
  virtual ~DifferenceNode();

  virtual bool intersect(const Ray& ray, Intersection& i) const;
};

#endif
