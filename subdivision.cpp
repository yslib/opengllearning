#include "subdivision.h"


Point3Df weightOneRing(SDVertex * vert, Float beta)
{
    std::vector<Point3Df> verticesOnTheRing(vert->valence());
    vert->oneRing(verticesOnTheRing.data());
    Point3Df p = (1 - vert->valence()*beta)*vert->point();

    for (int i = 0; i < verticesOnTheRing.size(); i++)
    {
        p += beta * verticesOnTheRing[i];
    }
    return p;
}

Point3Df weightBoundary(SDVertex * vert, Float beta)
{
    std::vector<Point3Df> verticesOnTheRing(vert->valence());
    vert->oneRing(verticesOnTheRing.data());
    Point3Df p = (1 - 2 * beta)*vert->point();
    p += beta * verticesOnTheRing.front();
    p += beta * verticesOnTheRing.back();
    return p;
}
