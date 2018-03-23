#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <unordered_map>
#include <string>
#include "core.h"
#include "geometry.h"
#include "appearance.h"
#include "model.h"


class Interaction;

class Shape
{
    std::shared_ptr<Material> m_material;
public:
    virtual AABB bound()const = 0;
    virtual Float area()const = 0;
    virtual bool intersect(const Ray & ray, Float * t, Interaction * iterac) = 0;
    void setMaterial(std::shared_ptr<Material> m)
    {
        m_material = m;
    }
    std::shared_ptr<Material> getMaterial()const
    {
        return m_material;
    }
    /*   virtual Interaction sample(const Point2f & u)const=0;
    virtual Float pdf(const Interaction & isect)const{
    return 1/area();
    }
    virtual Interaction sample(const Interaction & ref,const Point2f & u)const{
    return sample(u);
    }
    virtual pdf(const Interaction & ref,const Vector3f & wi)const{
    Ray ray(wi,ref.m_p);
    Float tHit;
    Interaction isect;
    if(intersect(ray,&tHit,&isect) == false)
    return 0;
    Float pdf = (ref.m_p-isect.m_p).lengthSquared()/(std::abs(Vector3f::dotProduct(isect.m_norm,-wi)*area());
    }*/

};

class TriangleMesh {
    std::unique_ptr<Point3f[]> m_vertices;
    int m_nVertex;
    std::unique_ptr<int[]> m_vertexIndices;
    int m_nIndex;
    std::unique_ptr<Vector3f[]> m_normals;
public:
    TriangleMesh(const Point3f * vertices, const Vector3f * normals, int nVertex, const int * vertexIndices, int nIndex, const Trans3DMat & trans)noexcept : m_nVertex(nVertex), m_nIndex(nIndex)
    {
        m_vertices.reset(new Point3f[nVertex]);
        m_vertexIndices.reset(new int[nIndex]);
        m_normals.reset(new Vector3f[nVertex]);
        for (int i = 0; i < nVertex; i++)
        {
            m_vertices[i] = trans * vertices[i];
        }
        for (int i = 0; i < nIndex; i++)
        {
            m_vertexIndices[i] = vertexIndices[i];
        }
        //create normals vertices
        for (int i = 0; i < nVertex; i++) {
            m_normals[i] = normals[i];
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
    const Point3f *getNormalsArray()const {
        return m_normals.get();
    }
    void transform(const Trans3DMat & trans)
    {
        for (int i = 0; i < m_nVertex; i++)
        {
            m_vertices[i] = trans * m_vertices[i];
        }
    }
    friend class Triangle;
};

class Triangle :public Shape {
    std::shared_ptr<TriangleMesh> m_sharedTriangles;
    const int * m_vertexIndices;
public:
    Triangle(std::shared_ptr<TriangleMesh> mesh, int indexOffset)
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
    //Interaction sample(const Point2f &u) const override{
    //    Point2f s = uniformSampleTriangle(u);
    //    Point3f p0 = m_sharedTriangles->m_vertexIndices[m_vertexIndices[0]];
    //    Point3f p1 = m_sharedTriangles->m_vertexIndices[m_vertexIndices[1]];
    //    Point3f p2 = m_sharedTriangles->m_vertexIndices[m_vertexIndices[2]];

    //    Point3f sp = p0*s[0] + p1*s[1] + (1-s[0]-s[1])*p2;

    //    ///TODO::
    //    //surface normal

    //    //error bound

    //    return isect;
    //}

    bool intersect(const Ray & ray, Float * t, Interaction * interac)override;
    static std::vector<std::shared_ptr<Shape>>
        createTriangleMesh(const Point3f * vertices, const Point3f * normals, int nVertex,
            const int * vertexIndices, int nIndex, std::unordered_map<int, std::string> & mtlName, MaterialReader & mtlReader,
            const Trans3DMat & trans)
    {
        assert(nIndex % 3 == 0);
        std::shared_ptr<TriangleMesh> mesh(new TriangleMesh(vertices, normals, nVertex, vertexIndices, nIndex, trans));
        std::vector<std::shared_ptr<Shape>> tris;
        for (int i = 0; i < nIndex / 3; i++)
        {
            tris.push_back(std::make_shared<Triangle>(mesh, i));
            std::string name = mtlName[i];
            Color kd = mtlReader[name]["Kd"];
            Color ks = mtlReader[name]["Ka"];
            Color ka = mtlReader[name]["Ka"];
            Color tf = mtlReader[name]["Tf"];
            Float ni = mtlReader[name]["Ni"][0];
            tris.back()->setMaterial(std::make_shared<Material>(kd, ks, ka, tf, ni));
        }

        return tris;
    }
};

#endif // SHAPE_H