#ifndef SAMPLER_H_
#define SAMPLER_H_
#include "core.h"
#include <cmath>

Vector3f uniformSampleHemiSphere(const Point2f & p);
constexpr Float uniformHemiSpherePdf();

Point2f uniformSampleDisk(const Point2f & p);
Float uniformDiskPdf(Float radius);

Vector3f cosineSampleHemiSphere(const Point2f & p);
Float cosineSampleHemiSpherePdf(Float cosTheta);

Vector3f uniformSampleCone(const Point2f & p, Float angle);
Float uniformSampleConePdf(Float angle);

inline Point2f uniformSampleTriangle(const Point2f & p);


inline int findMaxVector3fComponent(const Vector3f & v)
{
    int idx = 0;
    Float cMax = v[0];
    for (int i = 1; i < 3; i++)
    {
        if (cMax < v[i])
        {
            cMax = v[i];
            idx = i;
        }
    }
    return idx;
}
inline int findMaxPoint3fComponent(const Point3f & p)
{
    int idx = 0;
    Float cMax = p[0];
    for (int i = 1; i < 3; i++)
    {
        if (cMax < p[i])
        {
            cMax = p[i];
            idx = i;
        }
    }
    return idx;
}
inline Vector3f absOfVector3f(const Vector3f & v)
{
    return Vector3f(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
}
inline Point3f absOfPoint3f(const Point3f & p)
{
    return Point3f(std::abs(p[0]), std::abs(p[1]), std::abs(p[2]));
}

inline Vector3f permuteVector3f(const Vector3f & v, int kx, int ky, int kz)
{
    assert(kx >= 0 && kx < 3);
    assert(ky >= 0 && ky < 3);
    assert(kz >= 0 && kz < 3);
    assert((kx != ky) && (ky != kz) && (kz != kx));
    return Vector3f(v[kx], v[ky], v[kz]);
}
inline Point3f permutePoint3f(const Point3f & p, int kx, int ky, int kz)
{
    assert(kx >= 0 && kx < 3);
    assert(ky >= 0 && ky < 3);
    assert(kz >= 0 && kz < 3);
    assert((kx != ky) && (ky != kz) && (kz != kx));
    return Point3f(p[kx], p[ky], p[kz]);
}



#endif
