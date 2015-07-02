#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive();

  virtual bool intersect(const Ray& ray, Intersection& j) const
  {
    return false;
  }
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Cone : public Primitive {
public:
  virtual ~Cone();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Cylinder : public Primitive {
public:
  virtual ~Cylinder();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Cube : public Primitive {
public:
  virtual ~Cube();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Plane : public Primitive {
public:
  virtual ~Plane();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Torus : public Primitive {
public:
  virtual ~Torus();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class Disc : public Primitive {
public:
  virtual ~Disc();

  virtual bool intersect(const Ray& ray, Intersection& j) const;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierCone : public Primitive {
public:
  NonhierCone(const Point3D& pos, double height)
    : m_pos(pos), m_height(height)
  {
  }
  virtual ~NonhierCone();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_height;
};

class NonhierCylinder : public Primitive {
public:
  NonhierCylinder(const Point3D& pos, double radius, double height)
    : m_pos(pos), m_radius(radius), m_height(height)
  {
  }
  virtual ~NonhierCylinder();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_radius;
  double m_height;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_size;
};

class NonhierPlane : public Primitive {
public:
  NonhierPlane(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierPlane();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_size;
};

class NonhierTorus : public Primitive {
public:
  NonhierTorus(const Point3D& pos, double oradius, double iradius)
    : m_pos(pos)
    , m_oradius(oradius)
    , m_iradius(iradius)
  {
  }

  virtual ~NonhierTorus();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_oradius;
  double m_iradius;
};

class NonhierDisc : public Primitive {
public:
  NonhierDisc(const Point3D& pos, double radius)
    : m_pos(pos)
    , m_radius(radius)
  {
  }

  virtual ~NonhierDisc();

  virtual bool intersect(const Ray& ray, Intersection& j) const;

private:
  Point3D m_pos;
  double m_radius;
};

#endif
