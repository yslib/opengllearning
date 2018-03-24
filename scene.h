#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include "geometry.h"
#include "shape.h"

class Scene
{
    AABB m_worldBound;
    std::shared_ptr<Shape> m_shape;
public:
    Scene(std::shared_ptr<Shape> shape) :m_shape(shape) {}

    bool intersect(const Ray & ray, Float * t, Interaction * isect)const {
        return m_shape->intersect(ray, t, isect);
    }
    void setObject(std::shared_ptr<Shape> shape) {
        m_shape = shape;
    }
};

#endif // SCENE_H
