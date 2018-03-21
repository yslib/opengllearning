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

#endif
