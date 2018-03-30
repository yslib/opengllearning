#ifndef SAMPLER_H_
#define SAMPLER_H_
#include "core.h"
#include <cmath>
#include <random>
#include <ctime>

inline 
Vector3f uniformSampleHemiSphere(const Point2f &p)
{
    Float y = p[0];
    Float sinTheta = std::sqrt(std::max((Float)0, 1 - y * y));
    Float phi = 2 * PI*p[1];
    return Vector3f(std::cos(phi)*sinTheta, y, std::sin(phi)*sinTheta);
}
inline
constexpr Float uniformHemiSpherePdf()
{
    return 1 / (2 * PI);
}

inline
Point2f uniformSampleDisk(const Point2f &p)
{
    Float r = std::sqrt(p[0]);
    Float theta = 2 * PI*p[1];
    return Point2f(r*std::sin(theta), r*std::cos(theta));
}

inline
Float uniformDiskPdf(Float radius)
{
    return 1 / (PI*radius*radius);
}

inline
Vector3f cosineSampleHemiSphereWithShiness(const Point2f &p,Float shiness){
    Float phi, theta;
    phi = p[0] * 2 * PI;
    theta = shiness<0 ? asin(sqrt(p[1])) : acos(pow(p[1], 1 / (shiness + 1)));
    return Vector3f(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
}

inline
Vector3f cosineSampleHemiSphere(const Point2f &p)
{
    Point2f sampleFromDisk = uniformSampleDisk(p);
    Float y = std::sqrt(std::max(Float(0), 1 - sampleFromDisk[0] * sampleFromDisk[0] - sampleFromDisk[1] * sampleFromDisk[1]));
    return Vector3f(sampleFromDisk[0], y, sampleFromDisk[2]);
}

inline
Float cosineSampleHemiSpherePdf(Float cosTheta)
{
    return cosTheta / PI;
}


inline
Vector3f uniformSampleCone(const Point2f &p, Float angle)
{
    Float cosTheta = (Float(1) - p[0]) + p[0] * angle;
    Float sinTheta = std::sqrt(Float(1) - cosTheta * cosTheta);
    Float phi = p[1] * 2 * PI;
    return Vector3f(std::cos(phi)*sinTheta, cosTheta, std::sin(phi)*sinTheta);
}

inline bool russianRoulette(Float p,Float s){
    if (s<=p) return true;
    return false;
}

inline Vector3f reflection(const Vector3f & normal, const Vector3f & incidence) {
    Vector3f norm = normal.normalized();
    Vector3f inci = incidence.normalized();
    Float s = Vector3f::dotProduct(norm,inci);
    if(s>0)return (inci*std::sqrt(1-s*s)).normalized();
    return (inci - 2 * norm*(s));
}

inline 
Vector3f refraction(const Vector3f & normal, const Vector3f & incidence,Float ratioIOR) {
    Vector3f norm = normal.normalized();
    Vector3f inci = incidence.normalized();
    Float cosI = Vector3f::dotProduct(norm, inci);
    Float sinI = std::sqrt(1 - cosI*cosI);
    //if (sinI > ratioIOR) {
    //    //qDebug() << "rationIOR" << ratioIOR << " sinI" << sinI << " normal" << norm << " inci" << incidence << " dp:"<<Vector3f::dotProduct(norm,incidence);
    //    return Vector3f();
    //}

    Float cos2T = 1.0 - (1 - cosI * cosI)*(ratioIOR*ratioIOR);
    if (cos2T < 0)return Vector3f();
    Vector3f T = ratioIOR * inci - (ratioIOR*cosI + std::sqrt(cos2T))*norm;
    //if (cos2T > 0)return T;
    //return Vector3f();
    return T;
}

inline Float clamp(Float v,Float low,Float high){
    if(v > high)return high;
    if(v < low)return low;
    return v;
}

inline Color clamp(const Color & v,const Color & low,const Color & high){
    return Color(clamp(v[0],low[0],high[0]),clamp(v[1],low[1],high[1]),clamp(v[2],low[2],high[2]));
}


inline
Float uniformSampleConePdf(Float angle)
{
    return 1 / (2 * PI*(1 - angle));
}

inline
Point2f uniformSampleTriangle(const Point2f &p)
{
    Float su0 = std::sqrt(p[0]);
    return Point2f(1 - su0, p[1] * su0);
}


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
