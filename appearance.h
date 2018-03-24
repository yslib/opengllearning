#ifndef APPEARANCE_H
#define APPEARANCE_H
#include "geometry.h"
#include "core.h"
//#include "interaction.h"

enum BSDFType {
    BSDF_DIFFUSE = 1 << 0,
    BSDF_SPECULAR = 1 << 1,
    BSDF_REFRACTION = 1 << 2,
    BSDF_ALL = BSDF_DIFFUSE | BSDF_SPECULAR | BSDF_REFRACTION
};

class BSDF
{
    Color m_color;
    Vector3f m_n, m_t, m_s;
    BSDFType m_type;
    //n,m,s are normal and orthognal vectors
    //normal vector in shading coordinate system is (0,1,0)
public:
    BSDF(const Color & color, const Vector3f & n, const Vector3f & t, const Vector3f & s,BSDFType type) :m_color(color), m_n(n.normalized()), m_t(t.normalized()), m_s(s.normalized()),m_type(type) {}
    Color f(const Vector3f & wo, const Vector3f & wi)const { return m_color; }
    Vector3f worldToLocal(const Vector3f & v)const;
    Vector3f localToWorld(const Vector3f & v)const;
    Color sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf);
    BSDFType type()const { return m_type; }
    bool isType(BSDFType type) { return m_type == type;}

};

class Interaction;

class Material
{
    Color m_kd;
    Color m_ks;
    Color m_ka;
    Color m_tf;
    Float m_ni;
    Color m_color;
    Shape * m_pShape;
public:
    Material(const Color &kd, const Color &ks, const Color &ka, const Color &tf, Float ni) :
        m_kd(kd), m_ks(ks), m_ka(ka), m_tf(tf), m_ni(ni) {}
    void computeScatteringFunction(Interaction * isect);

};


#endif // APPEARANCE_H