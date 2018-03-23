#ifndef SUBDIVISION_H
#define SUBDIVISION_H


#include "geometry.h"
#include "core.h"

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
#endif // SUBDIVISION_H
