#include "scene.hpp"
#include <iostream>
#include <cctype>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  set_transform(m_trans.rotate(angle, (tolower(axis) == 'x') ? 1.0 : 0.0, (tolower(axis) == 'y') ? 1.0 : 0.0, (tolower(axis) == 'z') ? 1.0 : 0.0));
}

void SceneNode::scale(const Vector3D& amount)
{
  set_transform(m_trans.scale(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
  set_transform(m_trans.translate(amount));
}

bool SceneNode::is_joint() const
{
  return false;
}

bool SceneNode::intersect(const Ray& ray, Intersection& i) const
{
  // Transform the ray from WCS->MCS for this node
  Ray r(m_invtrans * ray.origin(), m_invtrans * ray.direction());

  bool intersects = false;
  for(auto child : m_children)
  {
    Intersection j;
    if(child->intersect(r, j))
    {
      // We need to see if this intersection point is closer than the previous intersection point
      // If it is than replace the previous intersection
      if(std::isinf(i.q[0]) || std::isinf(i.q[1]) || std::isinf(i.q[2]) || (j.q-r.origin()).length() < (i.q-r.origin()).length()) i = j;
      intersects = true;
    }
  }

  // If intersection occurs than transform the intersection point and the normal from MCS->WCS
  // We have to convert the intersection point from MCS->WCS and the normal from MCS->WCS
  // Normals must be multiplied by the transpose of the inverse to throw away scaling (no translations either, but the normal is 
  // a vector and vectors can't be translated) but preserve rotation
  if(intersects)
  {
    i.q = m_trans * i.q;
    i.n = transNorm(m_invtrans, i.n);
  }
  
  return intersects;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

bool GeometryNode::intersect(const Ray& ray, Intersection& i) const
{
  // Test for intersection
  // But first transform ray to geometry's model coordinates (inverse transform from WCS->MCS)
  Ray r(m_invtrans * ray.origin(), m_invtrans * ray.direction());

  Intersection k;
  bool intersects = m_primitive->intersect(r, k);
  if(intersects) 
  {
    i.q = m_trans * k.q;
    i.n = transNorm(m_invtrans, k.n);
    i.m = m_material;
  }

  return (intersects || SceneNode::intersect(ray, i));
}

GeometryNode::~GeometryNode()
{
}

ConstructiveSolidGeometryNode::ConstructiveSolidGeometryNode(const std::string& name, GeometryNode* A, GeometryNode* B)
  : GeometryNode(name, NULL)
  , m_A(A)
  , m_B(B)
{
}

ConstructiveSolidGeometryNode::~ConstructiveSolidGeometryNode()
{
}

UnionNode::UnionNode(const std::string& name, GeometryNode* A, GeometryNode* B)
  : ConstructiveSolidGeometryNode(name, A, B)
{
}

bool UnionNode::intersect(const Ray& ray, Intersection& i) const
{
  Ray r(m_invtrans * ray.origin(), m_invtrans * ray.direction());

  Intersection j, k;
  bool intersects_a = m_A->intersect(r, j);
  bool intersects_b = m_B->intersect(r, k);

  if(intersects_a || intersects_b)
  {
    i = (std::isinf(j.q[0]) || std::isinf(j.q[1]) || std::isinf(j.q[2]) || (k.q-r.origin()).length() < (j.q-r.origin()).length()) ? k : j;
    i.q = m_trans * i.q;
    i.n = transNorm(m_invtrans, i.n);
  }

  return (intersects_a || intersects_b || SceneNode::intersect(ray, i));
}

UnionNode::~UnionNode()
{
}

IntersectionNode::IntersectionNode(const std::string& name, GeometryNode* A, GeometryNode* B)
  : ConstructiveSolidGeometryNode(name, A, B)
{
}

bool IntersectionNode::intersect(const Ray& ray, Intersection& i) const
{
  Ray r(m_invtrans * ray.origin(), m_invtrans * ray.direction());

  Intersection j, k;
  bool intersects_a = m_A->intersect(r, j);
  bool intersects_b = m_B->intersect(r, k);

  if(intersects_a && intersects_b)
  {
    i = ((k.q-r.origin()).length() > (j.q-r.origin()).length()) ? k : j;
    i.q = m_trans * i.q;
    i.n = transNorm(m_invtrans, i.n);
  }

  return ((intersects_a && intersects_b) || SceneNode::intersect(ray, i));
}

IntersectionNode::~IntersectionNode()
{
}

DifferenceNode::DifferenceNode(const std::string& name, GeometryNode* A, GeometryNode* B)
  : ConstructiveSolidGeometryNode(name, A, B)
{
}

bool DifferenceNode::intersect(const Ray& ray, Intersection& i) const
{
  Ray r(m_invtrans * ray.origin(), m_invtrans * ray.direction());

  Intersection j, k;
  bool intersects_a = m_A->intersect(r, j);
  bool intersects_b = m_B->intersect(r, k);

  if(intersects_a && intersects_b)
  {
    i = ((k.q-r.origin()).length() > (j.q-r.origin()).length()) ? k : j;
    i.q = m_trans * i.q;
    i.n = transNorm(m_invtrans, i.n);
  }

  return ((intersects_a && intersects_b) || SceneNode::intersect(ray, i));
}

DifferenceNode::~DifferenceNode()
{
}

