#ifndef INTERACTION_H
#define INTERACTION_H
#include <memory>
#include "core.h"
#include "appearance.h"


class BSDF;

class Interaction
{
    Point3f  m_p;
    Vector3f  m_wo;
    Point3f m_norm;
    Float m_u;
    Float m_v;
    Shape* m_shape;
    std::shared_ptr<BSDF> m_bsdf;
    //BRDF
public:
    Interaction() {}
    Interaction(const Point3f & p, const Vector3f & wo, Float u, Float v) :m_p(p), m_wo(wo), m_u(u), m_v(v), m_bsdf(nullptr) {}
    const Point3f & intersectionPoint()const { return m_p; }
    const Vector3f & reflectDirection()const { return m_wo; }
    const Vector3f & normal()const { return m_norm; }
    Float u()const { return m_u; }
    Float v()const { return m_v; }
    std::shared_ptr<BSDF> bsdf()const { return m_bsdf; }
    friend class Shape;
    friend class Triangle;
    friend class Material;
};

#endif // INTERACTION_H