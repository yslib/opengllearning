#include "light.h"
Float VisibilityTester::distanceSquare() const
{
    return (m_p0.intersectionPoint()-m_p1.intersectionPoint()).lengthSquared();
}
bool VisibilityTester::occlude(const Scene & scene) const
{
    assert(m_p0.object() != nullptr);
    assert(m_p1.object() != nullptr);
    Interaction isect;
    Ray ray = m_p0.spawnRayTo(m_p1);
    Float t;
    scene.lights();
    if (scene.intersect(ray, &t, &isect) == false) {
        return false;
    }
    //return true;
    return !(m_p1.object() == isect.object());
    //return m_p0.object() != m_p1.object();
}
