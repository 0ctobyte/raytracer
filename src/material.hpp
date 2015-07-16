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
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture);
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture, const Image& bumpmap);
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

  void set_texture(const Image& texture) 
  {
    m_texture = texture;
    m_has_texture = true;
  }
  void set_bumpmap(const Image& bumpmap)
  {
    m_bumpmap = bumpmap;
    m_has_bumpmap = true;
  }

  virtual Colour diffuse(double u, double v) const;
  virtual Vector3D bump(const Vector3D& normal, const Vector3D& pu, const Vector3D& pv, double u, double v) const; 

protected:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  
  Image m_texture;
  Image m_bumpmap;
  bool m_has_texture;
  bool m_has_bumpmap;
};

#endif
