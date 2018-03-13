#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>
#include <cassert>

#include <limits>




constexpr Float LOWEST_Float_VALUE = std::numeric_limits<Float>::lowest();
constexpr Float MAX_Float_VALUE = std::numeric_limits<Float>::max();


inline int findMaxVector3fComponent(const Vector3f & v)
{
    int idx = 0;
    Float cMax = v[0];
    for(int i=1;i<3;i++)
    {
        if(cMax < v[i])
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
    for(int i=1;i<3;i++)
    {
        if(cMax < p[i])
        {
            cMax = p[i];
            idx = i;
        }
    }
    return idx;
}
inline Vector3f absOfVector3f(const Vector3f & v)
{
    return Vector3f(std::abs(v[0]), std::abs(v[1]),std::abs(v[2]));
}
inline Point3f absOfPoint3f(const Point3f & p)
{
    return Point3f(std::abs(p[0]), std::abs(p[1]),std::abs(p[2]));
}

inline Vector3f permuteVector3f(const Vector3f & v,int kx,int ky,int kz)
{
    assert(kx >= 0 && kx < 3);
    assert(ky >= 0 && ky < 3);
    assert(kz >= 0 && kz < 3);
    assert((kx != ky) && (ky != kz) && (kz != kx));
    return Vector3f(v[kx], v[ky], v[kz]);
}
inline Point3f permutePoint3f(const Point3f & p,int kx,int ky,int kz)
{
    assert(kx >= 0 && kx < 3);
    assert(ky >= 0 && ky < 3);
    assert(kz >= 0 && kz < 3);
    assert((kx != ky) && (ky != kz) && (kz != kx));
    return Point3f(p[kx], p[ky], p[kz]);
}
/*
 *
*/

class Ray
{
    Point3f m_o;
    Vector3f m_d;
    Float m_tMax;
public:
    Ray(const Point3f & d, const Point3f & o, Float t = MAX_Float_VALUE)noexcept :m_o(o), m_d(d), m_tMax(t){}
    Point3f operator()(float t)noexcept { return m_o + t * m_d; }
    friend class AABB;
    friend class Triangle;
    friend class Shape;
};

/*
 * Axis-aligned Bounding Box
 */
class AABB
{
    Point3f m_min;
    Point3f m_max;
public:
    AABB(const Point3f & p0 = Point3f(LOWEST_Float_VALUE, LOWEST_Float_VALUE, LOWEST_Float_VALUE),
        const Point3f & p1 = Point3f(MAX_Float_VALUE, MAX_Float_VALUE, MAX_Float_VALUE))noexcept :
        m_min(std::min(p0.x(), p1.x()), std::min(p0.y(), p1.y()), std::min(p0.z(), p1.z())),
        m_max(std::max(p0.x(), p1.x()), std::max(p0.y(), p1.y()), std::max(p0.z(), p1.z()))
    {}
    AABB(const Point3f & p) :m_min(p), m_max(p) {}

    const Point3f & operator[](int i)const {
        assert(i >= 0 && i < 2);
        if (i == 0)return m_min;
        if (i == 1)return m_max;
        return Point3f();
    }

    Point3f & operator[](int i)noexcept{
        return const_cast<Point3f&>(static_cast<const AABB &>(*this)[i]);
    }

    Point3f corner(int i)const {
        return Point3f(
            (*this)[i & 1].x(), (*this)[i & 2].y(), (*this)[i & 4].z()
        );
    }

    /*
     * Check whether it is intersected with a ray.
     * hit0 stores the nearest ray 
     * parameter and hit1 stores the farthest ray parameter
     */
    bool intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr)const noexcept
    {
        Float t0 = ray.m_tMax;
        Float t1 = 0;
        for (int i = 0; i < 3; i++)
        {
            Float inv = 1 / ray.m_d[i];
            Float tNear = (m_min[i] - ray.m_o[i])*inv;
            Float tFar = (m_max[i] - ray.m_o[i])*inv;
            if (tNear > tFar)std::swap(tNear, tFar);
            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar < t1 ? tFar : t1;
            if (t0 > t1)return false;
        }
        if(hit0 != nullptr)*hit0 = t0;
        if(hit1 != nullptr)*hit1 = t1;
        return true;
    }
    /*
     *Check whether a point is in the bound
     */
    bool inside(const Point3f & p)const {
        return (p.x() >= m_min.x() && p.y() <= m_max.x() &&
            p.y() >= m_min.y() && p.y() <= m_max.y() &&
            p.z() >= m_min.z() && p.z() <= m_max.z());
    }

    bool insideEx(const Point3f & p)const {
        return (p.x() >= m_min.x() && p.y() < m_max.x() &&
            p.y() >= m_min.y() && p.y() < m_max.y() &&
            p.z() >= m_min.z() && p.z() < m_max.z());
    }

    /*
     * Check whether the bounding box is 
     * intersected with another bounding box
     */
    bool isIntersectWith(const AABB & b)
    {
        return (m_max.x()>=b.m_min.x()&&b.m_max.x()>=m_min.x())&&
            (m_max.y()>=b.m_min.y()&&b.m_max.y()>=m_min.y())&&
            (m_max.z()>=b.m_min.z()&&b.m_max.z()>=m_min.z());
    }

    /*
     * return the common part of two bounding box
     */
    AABB intersectWith(const AABB & b)const {
        return AABB(Point3f(
            std::max(m_min.x(), b.m_min.x()),
            std::max(m_min.y(), b.m_min.y()),
            std::max(m_min.z(), b.m_min.z())
        ),
            Point3f(
                std::min(m_max.x(), b.m_max.x()),
                std::min(m_max.y(), b.m_max.y()),
                std::min(m_max.z(), b.m_max.z())
            ));
    }
    /*
     * Return a minimum bounding box containing the two bounding boxes
     */
    AABB unionWith(const AABB & b)const {
        return AABB(Point3f(
            std::min(m_min.x(), b.m_min.x()),
            std::min(m_min.y(), b.m_min.y()),
            std::min(m_min.z(), b.m_min.z())
        ),
            Point3f(
                std::max(m_max.x(), b.m_max.x()),
                std::max(m_max.y(), b.m_max.y()),
                std::max(m_max.z(), b.m_max.z())
            ));

    }
    /*
     * Return a minimun bounding box containing the 
     * bounding box and the point
     */
    AABB unionWith(const Point3f & p)const {
        return AABB(Point3f(
            std::min(m_min.x(), p.x()),
            std::min(m_min.y(), p.y()),
            std::min(m_min.z(), p.z())
        ),
            Point3f(
                std::max(m_max.x(), p.x()),
                std::max(m_max.y(), p.y()),
                std::max(m_max.z(), p.z())
            ));
    }
};

class Shape
{
public:
    
    virtual AABB bound()const = 0;
    virtual Float area()const = 0;
    virtual bool intersect(const Ray & ray, Float * t)const = 0;
};

class TriangleMesh {
    std::unique_ptr<Point3f[]> m_vertices;
    int m_nVertex;
    std::unique_ptr<int[]> m_vertexIndices;
    int m_nIndex;
public:
    TriangleMesh(const Point3f * vertices,int nVertex,const int * vertexIndices,int nIndex,const Trans3DMat & trans)noexcept:m_nVertex(nVertex),m_nIndex(nIndex)
    {
        m_vertices.reset(new Point3f[nVertex]);
        m_vertexIndices.reset(new int[nIndex]);
        for(int i=0;i<nVertex;i++)
        {
            m_vertices[i] = trans * vertices[i];
        }
        for(int i=0;i<nIndex;i++)
        {
            m_vertexIndices[i] = vertexIndices[i];
        }
    }
    const Point3f * getVerticesArray()const
    {
        return m_vertices.get();
    }
    int getVertexCount()const
    {
        return m_nVertex;
    }
    const int * getIndicesArray()const
    {
        return m_vertexIndices.get();
    }
    int getIndexCount()const
    {
        return m_nIndex;
    }
    void transform(const Trans3DMat & trans)
    {
        for(int i=0;i<m_nVertex;i++)
        {
            m_vertices[i] = trans*m_vertices[i];
        }
    }
    friend class Triangle;
};

class Triangle :public Shape {
    std::shared_ptr<TriangleMesh> m_sharedTriangles;
    const int * m_vertexIndices;
public:
    Triangle(std::shared_ptr<TriangleMesh> mesh,int indexOffset)
    {
        m_sharedTriangles = mesh;
        m_vertexIndices = &(mesh->m_vertexIndices[indexOffset * 3]);
    }
    AABB bound()const override
    {
        return AABB(m_sharedTriangles->m_vertices[m_vertexIndices[0]],
            m_sharedTriangles->m_vertices[m_vertexIndices[1]]).unionWith(
                m_sharedTriangles->m_vertices[m_vertexIndices[2]]);
    }
    Float area()const override
    {
        Vector3f v1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]] 
        - m_sharedTriangles->m_vertices[m_vertexIndices[0]];
        Vector3f v2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]]
            - m_sharedTriangles->m_vertices[m_vertexIndices[0]];
        return 0.5*Vector3f::dotProduct(v1, v2);

    }
    bool intersect(const Ray & ray, Float * t)const override
    {
        /*
         * This intersection checking is based on a transformation
         * that transforms the ray such that its origin is at (0,0,0)
         * in the transformed coordinate system and such that its 
         * direction is along the +z axis.
         */

        /*
         * First, translate the triangle vertices to coordinate 
         * whose original is the original of the ray
         */
        Point3f p0 = m_sharedTriangles->m_vertices[m_vertexIndices[0]] - ray.m_o;
        Point3f p1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]] - ray.m_o;
        Point3f p2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]] - ray.m_o;

        /*
         * Second, Find the maximum component of the direction of the ray,
         * and let the axis be the new coordinate's +z-axis
         */

        int iz = findMaxVector3fComponent(absOfVector3f(ray.m_d));
        int ix = (iz + 1) % 3;
        int iy = (iz + 2) % 3;

        Vector3f d = permuteVector3f(ray.m_d, ix, iy, iz);
        p0 = permutePoint3f(p0, ix, iy, iz);
        p1 = permutePoint3f(p1, ix, iy, iz);
        p2 = permutePoint3f(p2, ix, iy, iz);

        /*
         * Finally, the length of the direction of the ray should 
         * be unit length after transformation. So, a transformation 
         * must be performed.
         */
        Float sx = -d.x() / d.z();
        Float sy = -d.y() / d.z();
        Float sz = 1.f / d.z();
        p0.setX(sx*p0.z());
        p0.setY(sy*p0.z());
        p1.setX(sx*p1.z());
        p1.setY(sy*p1.z());
        p2.setX(sx*p2.z());
        p2.setY(sy*p2.z());


        Float e0 = p1.x()*p2.y() - p1.y()*p2.x();
        Float e1 = p2.x()*p0.y() - p2.y()*p0.x();
        Float e2 = p0.x()*p1.y() - p0.y()*p1.x();

        /*
         * the intersection point is not at the same side of all three edges 
         */
        if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0))
            return false;
        Float det = e0 + e1 + e2;
        if (det == 0)
            return false;




    }
    static std::vector<std::shared_ptr<Shape>> 
    createTriangleMesh(const Point3f * vertices,int nVertex,
        const int * vertexIndices,int nIndex,
        const Trans3DMat & trans)
    {
        assert(nIndex % 3 == 0);
        std::shared_ptr<TriangleMesh> mesh(new TriangleMesh(vertices, nVertex, vertexIndices, nIndex, trans));
        std::vector<std::shared_ptr<Shape>> tris;
        for(int i=0;i<nIndex/3;i++)
        {
            tris.push_back(std::make_shared<Triangle>(mesh,i));
        }
        return tris;
    }
};
class Scene
{
    AABB m_worldBound;
    std::shared_ptr<Shape> m_shape;
public:
    bool intersect(const Ray & ray, Float * t) {

    }
};

class BVHTreeAccelerator:public Shape
{
    
};
/*
 *Qt GUI
*/

class QPushButton;
class QLabel;
class QLineEdit;
class QSlider;
class Model;
class QTextEdit;
class OpenGLWidget;

class PathTracingDemo :
    public BaseDemoWidget
{
public:

    PathTracingDemo(QWidget * parent = nullptr);
    virtual ~PathTracingDemo();
private:
    QPushButton * m_openFileButton;
    QLabel * m_fileNameLabel;
    QLineEdit * m_fileNamesLineEdit;
    QWidget * m_displayWidget;

    OpenGLWidget * m_sceneDisplay;
    QLabel * m_resultDisplay;

    QLabel * m_sliderLabel;
    QSlider * m_slider;
    QTextEdit * m_textEdit;
    public slots:
    void onOpenFile();
    void onSamplesCountChanged(int value);
};
#endif
