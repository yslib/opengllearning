#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "core.h"


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



template<typename T> inline Vector3D<T> operator*(const T& s, const Vector3D<T> & v) {
    return v * s;
}

template<typename T> inline Vector3D<T> Abs(const Vector3D<T> & v) {
    return Vector3D<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
}


typedef Vector3D<int> Vector3Di;
typedef Vector3D<Float> Vector3Df;

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






/*
*
*/

class Ray
{
    Point3f m_o;
    Vector3f m_d;
    Float m_tMax;
    bool m_negDirection[3];
public:
    Ray(const Point3f & d, const Point3f & o, Float t = MAX_Float_VALUE)noexcept : m_o(o), m_d(d), m_tMax(t)
    {
        m_negDirection[0] = d.x() < 0;
        m_negDirection[1] = d.y() < 0;
        m_negDirection[2] = d.z() < 0;
    }
    Point3f operator()(float t)noexcept { return m_o + t * m_d; }
    const Point3f & original()const {
        return m_o;
    }
    const Vector3f & direction()const {
        return m_d;
    }
    void setMaxLength(Float t) {
        m_tMax = t;
    }

    friend class AABB;
    friend class Triangle;
    friend class Shape;
    friend class BVHTreeAccelerator;
};

class AABB
{
    Point3f m_min;
    Point3f m_max;
public:
    AABB() :m_min(MAX_Float_VALUE, MAX_Float_VALUE, MAX_Float_VALUE), m_max(LOWEST_Float_VALUE, LOWEST_Float_VALUE, LOWEST_Float_VALUE)
    {
        //construct a empty bounding box
    }
    AABB(const Point3f & p0, const Point3f & p1)noexcept :
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

    Point3f & operator[](int i)noexcept {
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
    /*inline function definitions for AABB*/
    bool intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr) const noexcept;

    Point3f center()const {
        return (m_min + m_max) / 2;
    }
    Vector3f diagnal()const
    {
        return m_max - m_min;
    }

    Float surfaceArea() const;

    /*
    *Check whether a point is in the bound
    */

    bool inside(const Point3f & p) const
    {
        return (p.x() >= m_min.x() && p.y() <= m_max.x() &&
            p.y() >= m_min.y() && p.y() <= m_max.y() &&
            p.z() >= m_min.z() && p.z() <= m_max.z());
    }

    bool insideEx(const Point3f & p) const
    {
        return (p.x() >= m_min.x() && p.y() < m_max.x() &&
            p.y() >= m_min.y() && p.y() < m_max.y() &&
            p.z() >= m_min.z() && p.z() < m_max.z());
    }

    /*
    * Check whether the bounding box is
    * intersected with another bounding box
    */

    bool isIntersectWith(const AABB & b);
    /*
    * return the common part of two bounding box
    */
    AABB intersectWith(const AABB & b) const;

    /*
    * Return a minimum bounding box containing the two bounding boxes
    */
    AABB unionWith(const AABB & b)const; 
    /*
    * Return a minimun bounding box containing the
    * bounding box and the point
    */
    AABB unionWith(const Point3f & p)const;


    friend class BVHTreeAccelerator;
};

inline 
Float AABB::surfaceArea() const
{
    Vector3f d = diagnal();
    if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
    Float area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
    return area;
}

inline 
bool AABB::isIntersectWith(const AABB & b)
{
    return (m_max.x() >= b.m_min.x() && b.m_max.x() >= m_min.x()) &&
        (m_max.y() >= b.m_min.y() && b.m_max.y() >= m_min.y()) &&
        (m_max.z() >= b.m_min.z() && b.m_max.z() >= m_min.z());
}

inline 
AABB AABB::unionWith(const Point3f & p)const {
    AABB a;
    a.m_min = Point3f(
        std::min(m_min.x(), p.x()),
        std::min(m_min.y(), p.y()),
        std::min(m_min.z(), p.z())
    );
    a.m_max = Point3f(
        std::max(m_max.x(), p.x()),
        std::max(m_max.y(), p.y()),
        std::max(m_max.z(), p.z())
    );
    return a;
}

inline 
AABB AABB::unionWith(const AABB & b) const
{
    AABB a;
    a.m_min = Point3f(
        std::min(m_min.x(), b.m_min.x()),
        std::min(m_min.y(), b.m_min.y()),
        std::min(m_min.z(), b.m_min.z())
    );
    a.m_max = Point3f(
        std::max(m_max.x(), b.m_max.x()),
        std::max(m_max.y(), b.m_max.y()),
        std::max(m_max.z(), b.m_max.z())
    );
    return a;
}
inline
AABB AABB::intersectWith(const AABB & b) const
{
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




#endif // GEOMETRY_H
