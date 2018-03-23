#include "sampler.h"

Vector3f uniformSampleHemiSphere(const Point2f &p)
{
    //y = cos(phi)
    Float y = p[0];
    //x = sin(theta)*cos(phi)
    Float sinTheta = std::sqrt(std::max((Float)0,1-y*y));
    Float x = sinTheta*y;
    Float phi = 2*PI*y;
    return Vector3f(std::cos(phi)*sinTheta,y,std::sin(phi)*sinTheta);
}

constexpr Float uniformHemiSpherePdf()
{
    return 1/(2*PI);
}
Point2f uniformSampleDisk(const Point2f &p)
{
    Float r = std::sqrt(p[0]);
    Float theta = 2*PI*p[1];
    return Point2f(r*std::sin(theta),r*std::cos(theta));
}
Float uniformDiskPdf(Float radius)
{
    return 1/(PI*radius*radius);
}
Vector3f cosineSampleHemiSphere(const Point2f &p)
{
    Point2f sampleFromDisk = uniformSampleDisk(p);
    Float y = std::sqrt(std::max(Float(0),1-sampleFromDisk[0]*sampleFromDisk[0]-sampleFromDisk[1]*sampleFromDisk[1]));
    return Vector3f(sampleFromDisk[0],y,sampleFromDisk[2]);
}
Float cosineSampleHemiSpherePdf(Float cosTheta)
{
    return cosTheta/PI;
}


Vector3f uniformSampleCone(const Point2f &p,Float angle)
{
    Float cosTheta = (Float(1)-p[0]) + p[0] *angle;
    Float sinTheta = std::sqrt(Float(1) - cosTheta*cosTheta);
    Float phi = p[1]*2*PI;
    return Vector3f(std::cos(phi)*sinTheta,cosTheta,std::sin(phi)*sinTheta);
}

Float uniformSampleConePdf(Float angle)
{
    return 1/(2*PI*(1-angle));
}


Point2f uniformSampleTriangle(const Point2f &p)
{
    Float su0 = std::sqrt(u[0]);
    return Point2f(1-su0,u[1]*su0);
}
