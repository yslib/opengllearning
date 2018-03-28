#include "appearance.h"
#include "interaction.h"
#include "utils.h"


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

Color BSDF::sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf,const Point2f & sample,BSDFType type)
{
    if (isType(type) == false)return Color(0.f,0.f,0.f);
    Color f;
    //norm must be normalized
    const auto & norm = m_n;
    switch (type)
    {
    case BSDF_DIFFUSE:
        *wi = -localToWorld(uniformSampleHemiSphere(sample));
        if (pdf)*pdf = 1.0 / (2 * PI);
        return m_color;
        break;
    case BSDF_SPECULAR:
    {
        *wi = 2 * (Vector3f::dotProduct(norm, wo))*norm-wo;
        if (pdf)*pdf = 1;
        return m_color;
    }
        break;
    case BSDF_REFRACTION:
    {
        ///TODO:how to judge wo is in the interior of the object or outerior of the object
        Float cosTheta = Vector3f::dotProduct(m_n, wo);
        Float n;
        Float fr, ft;
        if (cosTheta < 0) {
            //outer
            n = m_ni;
            cosTheta = -cosTheta;
            fersnel(cosTheta, n, &fr, &ft);
            //tracing reflection and refraction

        }
        else {
            //inter
            fersnel(cosTheta, n, &fr, &ft);
            n = 1.f/m_ni;
            assert(n <= 1.0);
            if (cosTheta < sqrt(1 - n * n)) {
                //full internal reflection
                if(pdf)*pdf = 1;
                *wi = -reflect(m_n, wo);
                return m_color;
            }
            else {

            }
        }
        
    }
        break;
    case BSDF_ALL:
        assert(false);
        break;
    default:
        assert(false);
        break;
    }
    return f;
}

void BSDF::fersnel(Float cosTheta,Float n,Float *fr,Float *ft)
{
    Float f0 = (n - 1)*(n - 1) / ((n + 1)*(n + 1));
    if (fr)*fr = f0;
    if (ft)*ft = f0 + (1 - f0)*std::pow(1 - cosTheta, 5.0);
}

void Material::computeScatteringFunction(Interaction * isect)
{
    BSDFType bsdfType;
    switch (m_type)
    {
    case MaterialType::Glass:
        bsdfType = BSDF_REFRACTION;
        break;
    case MaterialType::Mirror:
        bsdfType = BSDF_SPECULAR;
        break;
    case MaterialType::Metal:
        bsdfType = BSDF_DIFFUSE;
        break;
    default:
        //error
        exit(0);
        break;
    }
    isect->m_bsdf = std::make_shared<BSDF>(m_kd,isect->m_norm,isect->m_t,isect->m_s,BSDFType::BSDF_REFRACTION);
}
