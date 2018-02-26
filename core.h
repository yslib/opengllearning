#ifndef SHAPE_H
#define SHAPE_H


#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

template<typename T>
class Vector3D {
	T m_x, m_y, m_z;
public:
	Vector3D(const T &x = T(),const T &y = T(),const T& z= T()):m_x(x),m_y(y),m_z(z) {}
	Vector3D(const Vector3D<T> & v) :m_x(T(p.m_x)), m_y(T(p.m_y)), m_z(T(p.m_z)) {}
	Vector3D<T> operator+(const Vector3D<T> & v)const {
		return Vector3D<T>(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
	}

	Vector3D<T> & operator+=(const Vector3D<T> & v) {
		m_x += v.m_x;
		m_y += v.m_y;
		m_z += v.m_z;
		return *this;
	}

	Vector3D<T> operator*(T s)const {
		return Vector3D<T>(s*m_x, s*m_y, s*m_z);
	}
	Vector3D<T> & operator*=(T s) {
		m_x *= s;
		m_y *= s;
		m_z *= s;
		return *this;
	}

	Vector3D<T> operator/(T f)const {
		Float inv = static_cast<Float>(1) / f;
		return Vector3D<T>(m_x*inv, m_y*inv, m_z*inv);
	}

	Vector3D<T> & operator/(T f) {
		Float inv = static_cast<Float>(1) / f;
		m_x *= inv; m_y *= inv; m_z *= inv;
		return *this;
	}

	//unary operator
	Vector3D<T> operator-()const {
		return Vector3D<T>(-m_x, -m_y, -m_z);
	}

	T x()const { return m_x; }
	T y()const { return m_y; }
	T z()const { return m_z; }
    void x(const T & x) { m_x =x; }
	void y(const T & y) { m_y = y; }
	void z(const T & z) { m_z = z; }
};

typedef Vector3D<int> Vector3Di;
typedef Vector3D<Float> Vector3Df;


template<typename T> inline Vector3D<T> operator*(T s, const Vector3D<T> & v) {
	return v * s;
}

template<typename T> inline Vector3D<T> Abs(const Vector3D<T> & v) {
	return Vector3D<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
}

template<typename T>
class Point3D {
public:
	Point3D(const T &x = T(), const T &y = T(), const T& z = T()):m_x(x),m_y(y),m_z(z) {}
	template<typename U> explicit Point3D(const Point3D<U> & p) :m_x(T(p.m_x)),m_y(T(p.m_y)),m_z(T(p.m_z)) {}
	Point3D<T> operator+(const Vector3D<T> & v)const {
		return Point3D<T>(m_x + v.x(), m_y + v.y(), m_z + v.z());
	}
	Point3D<T> & operator+=(const Vector3D<T> & v) {
		m_x += v.x();
		m_y += v.y();
		m_z += v.z();
		return *this;
	}

	Vector3D<T> operator-(const Point3D<T> & p)const {
		return Vector3D<T>(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z);
	}
	Point3D<T> operator-(const Vector3D<T> & v) const{
		return Point3D<T>(m_x - v.x(), m_y - v.y(), m_z - v.z());
	}
	Point3D<T> & operator-=(const Vector3D<T> & v)const {
		m_x -= v.x();
		m_y -= v.y();
		m_z -= v.z();
		return *this;
	}
	T x()const { return m_x; }
	T y()const { return m_y; }
	T z()const { return m_z; }
	void x(const T & x) { m_x =x; }
	void y(const T & y) { m_y = y; }
	void z(const T & z) { m_z = z; }
private:
	T m_x, m_y, m_z;
};

typedef Point3D<int> Point3Di;
typedef Point3D<Float> Point3Df;



#endif // SHAPE_H