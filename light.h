#ifndef LIGHT_H
#define LIGHT_H
#include "core.h"
#include "shape.h"

class Light {
public:
};
class AreaLight {
    std::shared_ptr<Shape> m_shape;
public:
};
#endif // LIGHT_H