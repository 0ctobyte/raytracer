#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
  , m_has_texture(false)
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
  , m_texture(texture), m_has_texture(true)
{
}

PhongMaterial::PhongMaterial(const PhongMaterial* material, const Image& texture)
  : m_kd(material->diffuse()), m_ks(material->specular()), m_shininess(material->shininess())
  , m_texture(texture), m_has_texture(true)
{
}

PhongMaterial::~PhongMaterial()
{
}

const Colour PhongMaterial::diffuse(double u, double v) const
{
  if(!m_has_texture) return diffuse();

  int x = u * (double)m_texture.width();
  int y = v * (double)m_texture.height();

  return Colour(m_texture(x, y, 0), m_texture(x, y, 1), m_texture(x, y, 2));
}

