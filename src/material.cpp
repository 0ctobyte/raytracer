#include "material.hpp"
#include "perlin.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double ni)
  : m_kd(kd), m_ks(ks), m_shininess(shininess), m_ni(ni)
  , m_has_texture(false), m_has_bumpmap(false), m_perlin_noise(-1)
{
}

PhongMaterial::PhongMaterial(std::shared_ptr<const PhongMaterial> material)
  : m_kd(material->m_kd), m_ks(material->m_ks), m_shininess(material->m_shininess), m_ni(material->m_ni)
  , m_texture(material->m_texture), m_bumpmap(material->m_bumpmap)
  , m_has_texture(material->m_has_texture), m_has_bumpmap(material->m_has_bumpmap), m_bumpscale(material->m_bumpscale), m_perlin_noise(-1)
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

Colour PhongMaterial::diffuse(double x, double y, double z) const
{
  return m_kd * Perlin::marble(x, y, z);
}

Vector3D PhongMaterial::bump(const Vector3D& n, const Vector3D& pu, const Vector3D& pv, double u, double v) const
{
  if(!m_has_bumpmap) return n.normalized();

  auto bumpmap = [this](double u, double v) -> double { 
    double di = u * (double)(m_bumpmap.width()-1);
    double dj = v * (double)(m_bumpmap.height()-1);

    int i = di;
    int j = dj;

    // Wrap around if the coordinates are out of bounds
    int i1 = ((i + 1) >= m_bumpmap.width()) ? 0 : i + 1;
    int j1 = ((j + 1) >= m_bumpmap.height()) ? 0 : j + 1;

    // Get the barycetric coordinates between pixels
    double up = di - i;
    double vp = dj - j;

    // Bilinear interpolation between pixel values (weighted average of the surrounding pixels using the barycentric coordinates up, vp)
    return (1-up)*(1-vp)*m_bumpmap(i, j, 0) +
        (1-up)*(vp)*m_bumpmap(i, j1, 0) +
        (up)*(1-vp)*m_bumpmap(i1, j, 0) +
        (up)*(vp)*m_bumpmap(i1, j1, 0);
  };

  double e = std::numeric_limits<double>::epsilon();

  double fu = (bumpmap(u+e, v) - bumpmap(u-e, v)) / (2*e);
  double fv = (bumpmap(u, v+e) - bumpmap(u, v-e)) / (2*e);

  Vector3D D = (m_bumpscale / n.length()) * (fu * (n.cross(pv)) - fv * (n.cross(pu)));

  return (n + D).normalized();
}

