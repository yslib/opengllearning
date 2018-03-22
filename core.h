#ifndef SHAPE_H
#define SHAPE_H


#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <list>
#include <memory>



/*
 * Qt Headers
 */
#include <QDebug>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QColor>

#include <QMatrix4x4>


constexpr Float PI = 3.1415926535;

 /*
  * Vector2D
 */

template<typename T>
class Vector2D {
private:
    T m_x;
    T m_y;
public:
    explicit Vector2D(const T & x = T(), const T & y = T()) :m_x(x), m_y(y) {}
    explicit Vector2D(const Vector2D<T> & v) :m_x(v.m_x), m_y(v.m_y) {}
    Vector2D<T> operator+(const Vector2D<T> & v) {
        return Vector2D<T>(m_x + v.m_x, m_y + v.m_y);
    }
    Vector2D<T> & operator+=(const Vector2D<T> & v) {
        m_x += v.m_x;
        m_y += v.m_y;
        return *this;
    }
    Vector2D<T> operator-(const Vector2D<T> & v) {
        return Vector2D<T>(m_x - v.m_x, m_y - v.m_y);
    }
    Vector2D<T> & operator-=(const Vector2D<T> & v) {
        m_x -= v.m_x;
        m_y -= v.m_y;
        return *this;
    }
    Vector2D<T> operator*(const T & s) {
        return Vector2D<T>(s*m_x, s*m_y);
    }
    Vector2D<T> & operator*=(const T & s) {
        m_x *= s;
        m_y *= s;
        return *this;
    }
    Vector2D<T> operator -() {
        return Vector2D<T>(-m_x, -m_y);
    }

    template<typename X> friend class Point2D; //Vector2D can be accessed by all instances of Point2D
};


/*
 * Point2D
*/
template<typename T>
class Point2D {
private:
    T m_x;
    T m_y;
public:
    explicit Point2D(const T & x = T(), const T & y = T()) :m_x(x), m_y(y) {}
    explicit Point2D(const Point2D<T> & v) :m_x(v.m_x), m_y(v.m_y) {}
    Point2D<T> operator+(const Vector2D<T> & v) {
        return Point2D<T>(m_x + v.m_x, m_y + v.m_y);
    }
    Point2D<T> operator +(const Point2D<T> & p) {
        return Point2D<T>(m_x + p.m_x, m_y + p.m_y);
    }

    Point2D<T> & operator+=(const Vector2D<T> & v) {
        m_x += v.m_x;
        m_y += v.m_y;
        return *this;
    }

    Point2D<T> operator-(const Vector2D<T> & v) {
        return Point2D<T>(m_x - v.m_x, m_y - v.m_y);
    }
    Point2D<T> & operator-=(const Vector2D<T> & v) {
        m_x -= v.m_x;
        m_y -= v.m_y;
        return *this;
    }
    Point2D<T> operator*(const T & s) {
        return Point2D<T>(s*m_x, s*m_y);
    }
    Point2D<T> & operator*=(const T & s) {
        m_x *= s;
        m_y *= s;
        return *this;
    }
    Point2D<T> operator -() {
        return Point2D<T>(-m_x, -m_y);
    }
};


/*
 * Vector3D
*/
template<typename T>
class Vector3D {
    T m_x, m_y, m_z;
public:
    explicit Vector3D(const T &x = T(), const T &y = T(), const T& z = T()) :m_x(x), m_y(y), m_z(z) {}
    Vector3D(const Vector3D<T> & v) :m_x(T(v.m_x)), m_y(T(v.m_y)), m_z(T(v.m_z)) {}
    Vector3D<T> operator+(const Vector3D<T> & v)const {
        return Vector3D<T>(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
    }
    Vector3D<T> & operator+=(const Vector3D<T> & v) {
        m_x += v.m_x;
        m_y += v.m_y;
        m_z += v.m_z;
        return *this;
    }
    Vector3D<T> operator*(const T& s)const {
        return Vector3D<T>(s*m_x, s*m_y, s*m_z);
    }
    Vector3D<T> & operator*=(const T& s) {
        m_x *= s;
        m_y *= s;
        m_z *= s;
        return *this;
    }
    Vector3D<T> operator/(const T& f)const {
        Float inv = static_cast<Float>(1) / f;
        return Vector3D<T>(m_x*inv, m_y*inv, m_z*inv);
    }
    Vector3D<T> & operator/(const T& f) {
        Float inv = static_cast<Float>(1) / f;
        m_x *= inv; m_y *= inv; m_z *= inv;
        return *this;
    }
    //unary operator
    Vector3D<T> operator-()const {
        return Vector3D<T>(-m_x, -m_y, -m_z);
    }

    const T x()const { return m_x; }
    const T y()const { return m_y; }
    const T z()const { return m_z; }
    void x(const T & x) { m_x = x; }
    void y(const T & y) { m_y = y; }
    void z(const T & z) { m_z = z; }
};

typedef Vector3D<int> Vector3Di;
typedef Vector3D<Float> Vector3Df;


template<typename T> inline Vector3D<T> operator*(const T& s, const Vector3D<T> & v) {
    return v * s;
}

template<typename T> inline Vector3D<T> Abs(const Vector3D<T> & v) {
    return Vector3D<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
}

/*
 * Point3D
*/

template<typename T>
class Point3D {
public:
    Point3D(const T &x = T(), const T &y = T(), const T& z = T()) :m_x(x), m_y(y), m_z(z) {}
    template<typename U> explicit Point3D(const Point3D<U> & p) :m_x(T(p.m_x)), m_y(T(p.m_y)), m_z(T(p.m_z)) {}
    Point3D<T> operator+(const Vector3D<T> & v)const {
        return Point3D<T>(m_x + v.x(), m_y + v.y(), m_z + v.z());
    }
    Point3D<T> & operator+=(const Vector3D<T> & v) {
        m_x += v.x();
        m_y += v.y();
        m_z += v.z();
        return *this;
    }
    Point3D<T> operator+(const Point3D<T> & v)const {
        return Point3D<T>(m_x + v.x(), m_y + v.y(), m_z + v.z());
    }
    Point3D<T> & operator+=(const Point3D<T> & v) {
        m_x += v.x();
        m_y += v.y();
        m_z += v.z();
        return *this;
    }



    Point3D<T> operator*(const T & s)const
    {
        return Point3D<T>(s*m_x, s*m_y, s*m_z);
    }
    Point3D<T> & operator*=(const T & s)
    {
        m_x *= s;
        m_y *= s;
        m_z *= s;
        return *this;
    }

    Vector3D<T> operator-(const Point3D<T> & p)const {
        return Vector3D<T>(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z);
    }
    Point3D<T> operator-(const Vector3D<T> & v) const {
        return Point3D<T>(m_x - v.x(), m_y - v.y(), m_z - v.z());
    }
    Point3D<T> & operator-=(const Vector3D<T> & v)const {
        m_x -= v.x();
        m_y -= v.y();
        m_z -= v.z();
        return *this;
    }


    const T x()const { return m_x; }
    const T y()const { return m_y; }
    const T z()const { return m_z; }
    void x(const T & x) { m_x = x; }
    void y(const T & y) { m_y = y; }
    void z(const T & z) { m_z = z; }
private:
    T m_x, m_y, m_z;
};


template<typename T> inline Point3D<T> operator*(const T & s, const Point3D<T> & p)
{
    return p * s;
}

typedef Point3D<int> Point3Di;
typedef Point3D<Float> Point3Df;

typedef QMatrix4x4 Trans3DMat;
typedef QVector3D Point3f;
typedef QVector3D Vector3f;
typedef QVector2D Point2f;
typedef QVector2D Vector2f;

typedef QPoint Point2i;
typedef QPoint Vector2i;

typedef Point3f Color;


class Transform3DMatrix
{
public:

private:
    Float m_m[4][4];

};



/*
 * Ray
*/
//class Ray {
//public:
//    Ray(const Point3Df & o = Point3Df(),const Vector3Df & d = Vector3Df(),Float t = 0.0):m_o(o),m_d(d),m_t(t) {};
//    Point3Df operator()(Float t) { return m_o + t * m_d; }
//private:
//    Vector3Df m_d;
//    Point3Df m_o;
//    Float m_t;
//};
//

//Vertex for subdivision
//Face for subdivision
class SDFace;
class SDVertex;
class SDEdge;

class SDEdge {
public:
    SDEdge(SDVertex * v0 = nullptr, SDVertex * v1 = nullptr) :f0edgeNum(-1) {
        m_vertices[0] = std::min(v0, v1);
        m_vertices[1] = std::max(v0, v1);

        m_faces[0] = m_faces[1] = nullptr;
    }
    SDVertex * v0()const { return m_vertices[0]; }
    void setV0(SDVertex * v0) { m_vertices[0] = v0; }
    SDVertex * v1()const { return m_vertices[1]; }
    void setV1(SDVertex * v1) { m_vertices[1] = v1; }
    SDFace * f0()const { return m_faces[0]; }
    void setF0(SDFace * f0) { m_faces[0] = f0; }
    SDFace * f1()const { return m_faces[1]; }
    void setF1(SDFace * f1) { m_faces[1] = f1; }
    void setEdgeNum(int n) { assert(n >= 0 && n < 3); f0edgeNum = n; }
    int edgeNum() { return f0edgeNum; }

    bool operator<(const SDEdge & e)const
    {
        if (m_vertices[0] != e.m_vertices[0])return m_vertices[0] < e.m_vertices[0];
        return m_vertices[1] < e.m_vertices[1];
    }

private:
    SDVertex * m_vertices[2];
    SDFace * m_faces[2];
    int f0edgeNum;
};

//#define NEXT(i) (((i)+(1))%3)
//#define PREV(i) (((i)+2)%3)


inline int NEXT(int i) { return (i + 1) % 3; }
inline int PREV(int i) { return (i - 1 + 3) % 3; }


class SDFace {
public:
    //SDFace(const std::vector<SDVertex *> & vertices = std::vector<SDVertex*>(),
    //	const std::vector<SDFace *> faces = std::vector<SDFace*>()):m_vertices(vertices),m_faces(faces){
    //	assert(vertices.size() ==3||vertices.size() ==0);
    //	assert(faces.size() == 3 || faces.size() == 0);
    //	m_faces.resize(3);
    //	m_vertices.resize(3);
    //	m_children.resize(4);
    //}

    SDFace * f[3];
    SDVertex * v[3];
    SDFace * children[4];

    SDFace(SDVertex *v0 = nullptr,
        SDVertex * v1 = nullptr,
        SDVertex * v2 = nullptr,
        SDFace * f0 = nullptr,
        SDFace *f1 = nullptr,
        SDFace * f2 = nullptr,
        SDFace * c0 = nullptr,
        SDFace * c1 = nullptr,
        SDFace * c2 = nullptr,
        SDFace * c3 = nullptr
    )
    {
        f[0] = f0;
        f[1] = f1;
        f[2] = f2;
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        children[0] = c0;
        children[1] = c1;
        children[2] = c2;
        children[3] = c3;

    }


    //std::vector<SDVertex*> getVertexPointers()const
    //{
    //	return m_vertices;
    //}
    //void setVertexPointers(const std::vector<SDVertex*> & vertices)
    //{
    //	m_vertices = vertices;
    //}
    //std::vector<SDFace*> getAdjacentFacesPointers()const
    //{
    //	return m_faces;
    //}
    //std::vector<SDFace*> getChildrenFacesPointers()const
    //{
    //	return m_children;
    //}
    //void setAdjacentFacePointers(const std::vector<SDFace *> & faces)
    //{
    //	m_faces = faces;
    //}
    //void setChildrenFacePointers(const std::vector<SDFace*> & children)
    //{
    //	assert(children.size() == 4);
    //	m_children = children;
    //}
    int indexOfVertex(const SDVertex * vert)const {
        for (int i = 0; i < 3; i++)
        {
            if (v[i] == vert)
            {
                return i;
            }
        }
        return -1;
    }
    SDFace * nextFace(const SDVertex * vert)
    {
        int index = indexOfVertex(vert);
        return f[index];
    }
    SDFace * prevFace(const SDVertex * vert) {
        //return m_faces[(indexOfVertex(vert) - 1 + m_vertices.size()) % m_vertices.size()];
        int index = indexOfVertex(vert);
        return f[PREV(index)];
    }
    SDVertex * nextVert(const SDVertex * vert)
    {
        return v[NEXT(indexOfVertex(vert))];
    }
    SDVertex * prevVert(const SDVertex * vert)
    {
        return v[PREV(indexOfVertex(vert))];
    }
    SDVertex * otherVertex(const SDVertex * vert0, const SDVertex * vert1)const
    {
        for (int i = 0; i < 3; i++)
        {
            if (v[i] != vert0 && v[i] != vert1)
                return v[i];
        }
        return nullptr;
    }
    //private:
    //	std::vector<SDVertex*> m_vertices;
    //	std::vector<SDFace*> m_faces;
    // 	std::vector<SDFace*> m_children;
};

class SDVertex {
private:
    Point3Df m_p;
    SDFace * m_startFace;
    SDVertex * m_child;
    bool m_regular;
    bool m_boundary;
public:
    SDVertex(const Point3Df & p = Point3Df(0.0, 0.0, 0.0),
        SDFace * startFace = nullptr,
        SDVertex * child = nullptr,
        bool r = true,
        bool b = false) :
        m_p(p),
        m_startFace(startFace),
        m_child(child),
        m_regular(r),
        m_boundary(b) {}

    SDFace *  startFace() const { return m_startFace; }
    void setStartFace(SDFace * val) { m_startFace = val; }

    const Point3Df & point() const { return m_p; }
    void setPoint(const Point3Df & val) { m_p = val; }

    bool isRegular() const { return m_regular; }
    void regular(bool val) { m_regular = val; }


    SDVertex * child() const { return m_child; }
    void setChild(SDVertex * val) { m_child = val; }

    bool isBoundary() const { return m_boundary; }
    void boundary(bool val) { m_boundary = val; }

    int valence()const {
        int val = 1;
        SDFace * f = m_startFace;
        if (m_boundary == true) {
            //go through for two directions;
            while ((f = f->nextFace(this)) != nullptr)
                val++;
            f = m_startFace;
            while ((f = f->prevFace(this)) != nullptr)
                val++;
            return val + 1;
            //because the number of vertex arounding a boundary vertex is one more than the the number of faces around it
        }
        else {
            //This is simple
            while ((f = f->nextFace(this)) != m_startFace)
                val++;
            return val;
        }
    }

    void oneRing(Point3Df * pRing)
    {
        if (m_boundary == false)
        {
            SDFace * face = m_startFace;
            do
            {
                *pRing++ = face->nextVert(this)->point();
                //face->nextVert(this) is the vertex opposited to this, namely it is lying on the ring
                face = face->nextFace(this);
            } while (face != m_startFace);

        }
        else
        {
            SDFace * face = m_startFace, *f2;
            while ((f2 = face->nextFace(this)) != nullptr)
                face = f2;
            *pRing++ = face->nextVert(this)->point();		//the last vertex on the half-closed ring
            //then,go through the other way
            do
            {
                *pRing++ = face->prevVert(this)->point();
                face = face->prevFace(this);
            } while (face != nullptr);
        }
    }
};
inline Float beta(int valence)
{
    if (valence == 3)return 3.f / 16.f;
    else return 3.f / (8.f*valence);
}


Point3Df weightOneRing(SDVertex * vert, Float beta);
Point3Df weightBoundary(SDVertex * vert, Float beta);





#endif // SHAPE_H
