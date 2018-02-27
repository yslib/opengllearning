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


//Vertex for subdivision
//Face for subdivision

class SDFace;
class SDVertex;
class SDEdge;

class SDEdge {
public:
	SDEdge(SDVertex * v0 = nullptr, SDVertex * v1 = nullptr):f0edgeNum(-1) {
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

private:
	SDVertex * m_vertices[2];
	SDFace * m_faces[2];
	int f0edgeNum;
};


class SDFace{
public:
	SDFace(const std::vector<SDVertex *> & vertices = std::vector<SDVertex*>(),
		const std::vector<SDFace *> faces = std::vector<SDFace*>()):m_vertices(vertices),m_faces(faces){
	}
	std::vector<SDVertex*> vertices() { return m_vertices; }
	std::vector<SDFace*> adjacentFaces() { return m_faces; }
	std::vector<SDFace*> childrenFaces() { return m_children; }

private:
	std::vector<SDVertex*> m_vertices;
	std::vector<SDFace*> m_faces;
 	std::vector<SDFace*> m_children;
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
		m_boundary(b){}

	SDFace *  startFace() const { return m_startFace; }
	void setStartFace(SDFace * val) { m_startFace = val; }

	const Point3Df & point() const { return m_p; }
	void setPoint(const Point3Df & val) { m_p = val; }

	bool isRegular() const { return m_regular; }
	void regular(bool val) { m_regular = val; }


	SDVertex * Child() const { return m_child; }
	void setChild(SDVertex * val) { m_child = val; }

	bool isBoundary() const { return m_boundary; }
	void boundary(bool val) { m_boundary = val; }
};


#endif // SHAPE_H