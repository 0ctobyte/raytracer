#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include <memory>
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
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double ni, double glossiness);
  PhongMaterial(std::shared_ptr<const PhongMaterial> material);
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
  double ni() const
  {
    return m_ni;
  }
  double glossiness() const
  {
    return m_glossiness;
  }

  void set_texture(const Image& texture) 
  {
    m_texture = texture;
    m_has_texture = true;
  }
  void set_bumpmap(const Image& bumpmap, double bumpscale)
  {
    m_bumpmap = bumpmap;
    m_bumpscale = bumpscale;
    m_has_bumpmap = true;
  }

  virtual Colour diffuse(double u, double v) const;
  virtual Vector3D bump(const Vector3D& normal, const Vector3D& pu, const Vector3D& pv, double u, double v) const; 

protected:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  double m_ni;
  double m_glossiness;
  
  Image m_texture;
  Image m_bumpmap;
  bool m_has_texture;
  bool m_has_bumpmap;
  double m_bumpscale; // So bump mapping doesn't look right unless we scale it down. I must be doing something wrong...
};

#endif
