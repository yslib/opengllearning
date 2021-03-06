#include "shape.h"
#include "interaction.h"

bool Triangle::intersect(const Ray & ray, Float * t, Interaction * interac)const
{
    /*
    * This ray-triangle intersection algorithm is from
    * "Fast, Minimum Storage Ray-Triangle Intersection"
    * Tomas Moller & Ben Trumbore (1997) Fast, Minimum
    * Storage Ray-Triangle Intersection, Journal of Graphics
    * Tools, 2:1, 21-28, DOI: 10.1080/10867651.1997.10487468
    *
    */
    //qDebug() << "enter;";
    const Point3f & p0 = m_sharedTriangles->m_vertices[m_vertexIndices[0]];
    const Point3f & p1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]];
    const Point3f & p2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]];

    const Point3f & D = ray.m_d;
    Vector3f T;
    Vector3f E1 = p1 - p0;
    Vector3f E2 = p2 - p0;
    Vector3f P = Vector3f::crossProduct(D, E2);

    Float det = Vector3f::dotProduct(P, E1);
    if (det > 0) {
        T = ray.m_o - p0;
    }
    else {
        T = p0 - ray.m_o;
        det = -det;
    }
    //qDebug() << "det:" << det;
    if (det < 0.000001)
        return false;
    //qDebug() << "det";
    Float u, v;
    //u is the parameter coresponding to p1
    //v is the parameter coresponding to p2
    u = Vector3f::dotProduct(P, T);
    if (u<0.0 || u>det) {
        //u > 1, invalid
        return false;
    }
    //qDebug() << "2222222222";
    Vector3f Q = Vector3f::crossProduct(T, E1);
    v = Vector3f::dotProduct(D, Q);
    if (v < 0.0 || v + u>det) {
        // v > 1 ,invalid
        return false;
    }
    //qDebug() << "32333333333333";
    Float tt = Vector3f::dotProduct(E2, Q);

    Float inv = 1.0f / det;
    tt *= inv;
    //if (std::isnan(tt)) {
    //    qDebug() << "tt is nan: D" << D << " E2" << E2 << " E1" << " det" << det;
    //}
    if (tt < 0)return false;
    //qDebug() << "66666666666";
    if (tt > ray.m_tMax)
        return false;
    //qDebug() << "555555";
    u *= inv;
    v *= inv;

    //evaluate intersection information
    if (interac != nullptr) {
        interac->m_p = ray.m_o + ray.m_d*tt;
        interac->m_u = u;
        interac->m_v = v;

        //a normal of intersction should be interpulated
        Vector3f p0Norm = m_sharedTriangles->m_normals[m_vertexIndices[0]];
        Vector3f p1Norm = m_sharedTriangles->m_normals[m_vertexIndices[1]];
        Vector3f p2Norm = m_sharedTriangles->m_normals[m_vertexIndices[2]];

        Vector3f norm = (u * p1Norm + v * p2Norm + (1 - v - u)*p0Norm).normalized();
        interac->m_norm = norm;

        //create two tangent vectors perpendicular to the normal
        Vector3f t= Vector3f(-norm.z(), 0, norm.x()).normalized();
        Vector3f s = Vector3f::crossProduct(t,norm).normalized();
        interac->m_t = t;
        interac->m_s = s;
        interac->m_pShape = this;

        getMaterial()->computeScatteringFunction(interac);
    }
    //qDebug() << "isect:" << tt;
    if(tt<0.00001)return false;
    //qDebug() << "77777777777";
    if (t)*t = tt;
    return true;
}



//bool Sphere::intersect(const Ray & ray, Float * t, Interaction * iterac) const
//{

//    return false;
//}

bool Sphere::intersect(const Ray & ray, Float * t, Interaction * iterac) const
{
    //const Vector3f & D = ray.direction();
    //const Vector3f & CO = ray.original() - m_center;
    //Float a = Vector3f::dotProduct(D, D);
    //Float b = 2 * Vector3f::dotProduct(D, CO);
    //Float c = Vector3f::dotProduct(CO, CO) - m_radius * m_radius;
    //Float t1, t2;
    //Float eps = 0.00001;
    //if (quadraticEquation(a, b, c, t1, t2) == false)return false;
    //if (t1 > t2)std::swap(t1, t2);
    //if (t1 < eps) {
    //    if (t2 < eps)return false;
    //    if (t)*t = t2;
    //    return true;
    //}
    //if (t)*t = t1;
    //return true;
    Vector3f rd = ray.direction().normalized();
    Vector3f op = m_center - ray.original();
    Float res, eps = 1e-4;
    Float b = Vector3f::dotProduct(op, rd);
    Float det = b * b - Vector3f::dotProduct(op, op) + m_radius * m_radius;
    if (det < 0)
        return false; 
    else 
        det = std::sqrt(det);
    if ((res = (b - det)) > eps) {
        if (t)*t = res;
        return true;
    }
    else if ((res = (b + det)) > eps) {
        if (t)*t = res;
        return true;
    }
    return false;
}

Interaction Sphere::sample(const Point2f & u) const
{
    Interaction inter;
    Vector3f norm = uniformSampleSphere(u);
    //point
    inter.m_p = norm + m_center;
    //norm
    inter.m_norm = norm;
    inter.m_pShape = this;

    return inter;
}

