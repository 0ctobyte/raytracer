#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();
  
  const Colour& diffuse() const
  {
    return m_kd;
  }
  const Colour& specular() const
  {
    return m_ks;
  }
  double shininess() const
  {
    return m_shininess;
  }

private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};


#endif
