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
    Vector3f m_n, m_t, m_s;
    BSDFType m_type;
    Color m_ka;
    Color m_ks;
    Color m_kd;
    Float m_ni;
    Float m_ns;
    Color m_tf;
    Color m_e;
    //n,m,s are normal and orthognal vectors
    //normal vector in shading coordinate system is (0,1,0)
public:
    BSDF(const Color & kd,
        const Color & ks,
        const Color & ka,
        const Color & tf,
        Float ni,
        Float ns,
        const Color & e,
        const Vector3f & n, 
        const Vector3f & t, 
        const Vector3f & s,
        BSDFType type) :m_kd(kd),m_ka(ka),m_ks(ks),m_ni(ni),m_tf(tf),m_ns(ns),m_e(e), m_n(n.normalized()), m_t(t.normalized()), m_s(s.normalized()),m_type(type) {
        createCoordinateSystem(m_n, m_t, m_s);
    }
    //Color f(const Vector3f & wo, const Vector3f & wi)const { return m_color; }
    Vector3f worldToLocal(const Vector3f & v)const;
    Vector3f localToWorld(const Vector3f & v)const;
    void createCoordinateSystem(const Vector3f &N, Vector3f &t, Vector3f &s);
    Color sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf,const Point2f & sample,BSDFType type);
    Color emmision()const{return m_e;}
    BSDFType type()const { return m_type; }
    bool isType(BSDFType type) { return m_type == type;}
private:
    void fersnel(Float cosTheta,Float n,Float * fr,Float * ft);
};

class Interaction;


enum class MaterialType {
    Glass,
    Mirror,
    Metal
};

class Shape;
class Material
{
public:
    Color m_kd;
    Color m_ks;
    Color m_ka;
    Color m_ke;
    Color m_tf;
    Float m_ni;
    Float m_ns;
    Color m_color;
    const Shape * m_pShape;
    MaterialType m_type;
public:
    Material(const Color &kd, const Color &ks, const Color &ka, const Color & ke,const Color &tf, Float ni,Float ns,MaterialType type) :
        m_kd(kd), m_ks(ks), m_ka(ka),m_ke(ke), m_tf(tf), m_ni(ni),m_ns(ns),m_type(type) {}
    void computeScatteringFunction(Interaction * isect);
};


#endif // APPEARANCE_H
