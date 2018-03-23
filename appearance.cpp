#include "appearance.h"
#include "interaction.h"

Color BSDF::f(const Vector3f & wo, const Vector3f & wi) const
{
    return Color();
}

Vector3f BSDF::worldToLocal(const Vector3f & v) const
{
    return Vector3f(Vector3f::dotProduct(v, m_s), Vector3f::dotProduct(v, m_n), Vector3f::dotProduct(v, m_t));
}

Vector3f BSDF::localToWorld(const Vector3f & v) const
{
    return Vector3f(
        m_s.x()*v.x() + m_n.x()*v.y() + m_t.x()*v.z(),
        m_s.y()*v.x() + m_n.y()*v.y() + m_t.y()*v.z(),
        m_s.z()*v.x() + m_n.z()*v.y() + m_t.z()*v.z()
    );
}

Color BSDF::sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf)
{
    if (pdf)*pdf = 1 / (2 * PI);
    return m_color;
}

void Material::computeScatteringFunction(Interaction * isect)
{
    //interact->m_bsdf = std::make_shared<BSDF>(m_kd,interact->m_norm);
}
