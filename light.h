#ifndef LIGHT_H
#define LIGHT_H
#include "core.h"
#include "shape.h"
#include "geometry.h"
#include "interaction.h"
#include "scene.h"


class Scene;

class VisibilityTester {
    Interaction m_p0, m_p1;
public:
    VisibilityTester() {}
    VisibilityTester(const Interaction & p0, const Interaction & p1) :m_p0(p0), m_p1(p1) {}
    bool occlude(const Scene & scene)const; 
};


class Light {
public:
    virtual Color sampleLi(const Interaction & ref, Vector3f * wi, Float*pdf, const Point2f &u,VisibilityTester * vis)const = 0;
    virtual Float pdfLi(const Interaction & ref, const Vector3f & wi)const = 0;
    virtual ~Light() {}
};
class AreaLight :Light {
    std::shared_ptr<Shape> m_shape;
    Color m_intensity;
public:
    explicit AreaLight(const std::shared_ptr<Shape> & s, const Color & c) :m_shape(s), m_intensity(c) {}

    //sample a light to a surface point from other object, giving direction and pdf
    virtual Color sampleLi(const Interaction & ref, Vector3f * wi, Float*pdf, const Point2f &u,VisibilityTester * vis)const override {

        //shape will overlook the ref
        //see the definition of Shape
        Interaction isectWithLight = m_shape->sample(ref, u);
        *wi = (isectWithLight.m_p - ref.m_p).normalized();
        *pdf = m_shape->pdf(ref, *wi);

        ///TODO:Visibility Test
        *vis = VisibilityTester(ref, isectWithLight);
        return L(isectWithLight, -*wi);
    }
    //evaluate the pdf of the sampling a light along wi from other surface point
    virtual Float pdfLi(const Interaction & ref, const Vector3f & wi)const override { return m_shape->pdf(ref, wi); }

    Color L(const Interaction & isect, const Vector3f & w)const { return Vector3f::dotProduct(isect.m_norm, w) > 0 ? m_intensity : Color(0, 0, 0); }
    virtual ~AreaLight() {}
};



#endif // LIGHT_H