#include "light.h"

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
    return m_p0.object() != m_p1.object();
}
