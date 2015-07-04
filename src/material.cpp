#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
  , m_has_texture(false), m_has_bumpmap(false)
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
  , m_texture(texture), m_has_texture(true)
  , m_has_bumpmap(false)
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const Image& texture, const Image& bumpmap)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
  , m_texture(texture), m_bumpmap(bumpmap)
  , m_has_texture(true), m_has_bumpmap(true)
{
}

PhongMaterial::PhongMaterial(std::shared_ptr<const PhongMaterial> material)
  : m_kd(material->m_kd), m_ks(material->m_ks), m_shininess(material->m_shininess)
  , m_texture(material->m_texture), m_bumpmap(material->m_bumpmap)
  , m_has_texture(material->m_has_texture), m_has_bumpmap(material->m_has_bumpmap)
{
}

PhongMaterial::~PhongMaterial()
{
}

Colour PhongMaterial::diffuse(double u, double v) const
{
  if(!m_has_texture) return diffuse();

  double di = u * (double)(m_texture.width()-1);
  double dj = v * (double)(m_texture.height()-1);

  int i = di;
  int j = dj;

  // Wrap around if the coordinates are out of bounds
  int i1 = ((i+1) >= m_texture.width()) ? 0 : i+1;
  int j1 = ((j+1) >= m_texture.height()) ? 0 : j+1;

  // Get the barycentric coordinates between pixels
  double up = di - i;
  double vp = dj - j;

  // Bilinear interpolation between pixel values, basically a weighted average of the sourrounding pixels
  // using the barycentric coordinates up, vp
  return (1-up)*(1-vp)*Colour(m_texture(i, j, 0), m_texture(i, j, 1), m_texture(i, j, 2)) +
    (1-up)*(vp)*Colour(m_texture(i, j1, 0), m_texture(i, j1, 1), m_texture(i, j1, 2)) +
    (up)*(1-vp)*Colour(m_texture(i1, j, 0), m_texture(i1, j, 1), m_texture(i1, j, 2)) +
    (up)*(vp)*Colour(m_texture(i1, j1, 0), m_texture(i1, j1, 1), m_texture(i1, j1, 2));
}

Vector3D PhongMaterial::bump(const Vector3D& normal) const
{
  return normal;
}

