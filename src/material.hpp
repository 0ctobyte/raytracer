#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "image.hpp"

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
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture);
  PhongMaterial(const PhongMaterial* material, const Image& texture);
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

  virtual const Colour diffuse(double u, double v) const;

protected:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  
  Image m_texture;
  bool m_has_texture;
};

#endif
