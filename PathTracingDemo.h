#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>
#include <cassert>

#include <limits>
#include <algorithm>
#include <iterator>


class Interaction;

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
    const Point3f & original()const{
        return m_o;
    }
    const Vector3f & direction()const{
        return m_d;
    }
    void setMaxLength(Float t){
        m_tMax = t;
    }
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
    AABB():m_min(MAX_Float_VALUE,MAX_Float_VALUE,MAX_Float_VALUE),m_max(LOWEST_Float_VALUE,LOWEST_Float_VALUE,LOWEST_Float_VALUE)
    {
        //construct a empty bounding box
    }
    AABB(const Point3f & p0,const Point3f & p1)noexcept :
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
    Point3f center()const{
        return (m_min+m_max)/2;
    }
    Vector3f diagnal()const
    {
        return m_max-m_min;
    }
    Float surfaceArea()const{
        Vector3f d = diagnal();
        Float area = (d[0]*d[1]+d[1]*d[2]+d[2]*d[0])*2;
        return area;
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


    friend class BVHTreeAccelerator;
};

class Shape
{
public:
    
    virtual AABB bound()const = 0;
    virtual Float area()const = 0;
    virtual bool intersect(const Ray & ray, Float * t,Interaction * iterac)= 0;
};

class TriangleMesh {
    std::unique_ptr<Point3f[]> m_vertices;
    int m_nVertex;
    std::unique_ptr<int[]> m_vertexIndices;
    int m_nIndex;
    std::unique_ptr<Vector3f[]> m_normals;
public:
    TriangleMesh(const Point3f * vertices,const Vector3f * normals,int nVertex,const int * vertexIndices,int nIndex,const Trans3DMat & trans)noexcept:m_nVertex(nVertex),m_nIndex(nIndex)
    {
        m_vertices.reset(new Point3f[nVertex]);
        m_vertexIndices.reset(new int[nIndex]);
        m_normals.reset(new Vector3f[nVertex]);
        for(int i=0;i<nVertex;i++)
        {
            m_vertices[i] = trans * vertices[i];
        }
        for(int i=0;i<nIndex;i++)
        {
            m_vertexIndices[i] = vertexIndices[i];
        }
        //create normals vertices
        for(int i=0;i<nVertex;i++){
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
    const Point3f *getNormalsArray()const{
        return m_normals.get();
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


class Interaction
{
    Point3f  m_p;
    Vector3f  m_wo;
    Point3f m_norm;
    Float m_u;
    Float m_v;
    Shape * m_shape;

    //BRDF
public:
    Interaction(){}
    Interaction(const Point3f & p, const Vector3f & wo, Float u, Float v) :m_p(p), m_wo(wo), m_u(u), m_v(v) {}
    const Point3f & intersectionPoint()const { return m_p; }
    const Vector3f & reflectDirection()const { return m_wo; }
    Float u()const { return m_u; }
    Float v()const { return m_v; }

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


    bool intersect(const Ray & ray, Float * t,Interaction * interac)override
    {
        /*
         * This ray-triangle intersection algorithm is from
         * "Fast, Minimum Storage Ray-Triangle Intersection"
         * Tomas Moller & Ben Trumbore (1997) Fast, Minimum
         * Storage Ray-Triangle Intersection, Journal of Graphics
         * Tools, 2:1, 21-28, DOI: 10.1080/10867651.1997.10487468
         *
        */
        const Point3f & p0 = m_sharedTriangles->m_vertices[m_vertexIndices[0]];
        const Point3f & p1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]];
        const Point3f & p2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]];

        const Point3f & D = ray.m_d;
        Vector3f T;
        Vector3f E1 = p1-p0;
        Vector3f E2 = p2 - p0;
        Vector3f P = Vector3f::crossProduct(D,E2);

        Float det = Vector3f::dotProduct(P,E1);
        if(det>0){
            T = ray.m_o-p0;
        }else{
            T = p0-ray.m_o;
            det =-det;
        }
        if(det < 0.0001)
            return false;

        Float u,v;
        u = Vector3f::dotProduct(P,T);
        if(u<0.0||u>det){
            //u > 1, invalid
            return false;
        }
        Vector3f Q = Vector3f::crossProduct(T,E1);
        v = Vector3f::dotProduct(Q,D);
        if(v < 0.0||v>det){
            // v > 1 ,invalid
            return false;
        }

        if(t != nullptr)*t = Vector3f::dotProduct(E2,Q);

        Float inv = 1.0f/det;
        (*t)*=inv;
        if(*t > ray.m_tMax)
            return false;
        u*=inv;
        v*=inv;
        interac->m_u = v;
        interac->m_v = v;
        interac->m_norm = Vector3f::crossProduct(p1 - p0, p2 - p0);
        interac->m_shape = this;

        return true;
    }
    static std::vector<std::shared_ptr<Shape>> 
    createTriangleMesh(const Point3f * vertices,const Point3f * normals,int nVertex,
        const int * vertexIndices,int nIndex,
        const Trans3DMat & trans)
    {
        assert(nIndex % 3 == 0);
        std::shared_ptr<TriangleMesh> mesh(new TriangleMesh(vertices, normals,nVertex, vertexIndices, nIndex, trans));
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
    Scene(std::shared_ptr<Shape> shape):m_shape(shape){}
    bool intersect(const Ray & ray, Float * t) {

    }
};
class BVHTreeAccelerator:public Shape
{
    class BVHNode{
    public:
      int m_nShape;
      int m_shapeOffset;
      int m_splitAxis;
      AABB m_bound;
      std::unique_ptr<BVHNode> m_left;
      std::unique_ptr<BVHNode> m_right;
      BVHNode(){}
      BVHNode(std::unique_ptr<BVHNode> left,std::unique_ptr<BVHNode> right,int shapeOffset,
              int splitAxis,
               int nshape,
               const AABB & b = AABB()):
          m_nShape(nshape),
          m_shapeOffset(shapeOffset),
          m_splitAxis(splitAxis),
          m_bound(b){}
    };
    std::vector<std::shared_ptr<Shape>> m_shapes;
    std::unique_ptr<BVHNode> m_root;
    Float m_tMin;
public:
    BVHTreeAccelerator(const std::vector<std::shared_ptr<Shape>> & shapes):m_shapes(std::move(shapes)){
        std::vector<std::shared_ptr<Shape>> orderedShapes;
        m_root = recursiveBuild(m_shapes,0,m_shapes.size(),orderedShapes);
        m_shapes.swap(orderedShapes);
    }
    bool intersect(const Ray & ray,Float * t,Interaction * interac)override{
        m_tMin = MAX_Float_VALUE;
        if(recursiveIntersect(m_root.get(),ray,interac)){
            if(t)*t = m_tMin;
            return true;
        }
        return false;
    }
private:

    bool recursiveIntersect(const BVHNode * root,const Ray & ray,Interaction * interac){
        //If the BVH is empty or there is no intersection with current node
        if(root == nullptr || root->m_bound.intersect(ray,nullptr,nullptr) == false)
            return false;
        //Interior node
        if(root->m_nShape != -1){
            bool interLeft =  recursiveIntersect(root->m_left.get(),ray,interac);
            bool interRight =  recursiveIntersect(root->m_right.get(),ray,interac);
            return interLeft || interRight;
        }else{
            //Leaf node, check and find the nearest intersection
            bool isect = false;
            for(int i = 0;i<root->m_nShape;i++){
                Float t;
                if(m_shapes[i+root->m_shapeOffset]->bound().intersect(ray,&t) == true){
                    Interaction inter;
                    m_shapes[i + root->m_shapeOffset]->intersect(ray, &t, &inter);
                    m_tMin = std::min(m_tMin,t);
                    if(m_tMin > t)
                    {
                        m_tMin = t;
                        *interac = inter;
                    }
                    isect = true;
                }
            }
            return isect;
        }
    }

    std::unique_ptr<BVHNode> recursiveBuild(std::vector<std::shared_ptr<Shape>> & shapes,
                             int begin,int end,
                             std::vector<std::shared_ptr<Shape>> & orderedShapes){
        int currentNodeCount = end - begin;
        int offset = orderedShapes.size();
        AABB bound;
        for(int i=begin;i<end;i++){
            bound.unionWith(shapes[i]->bound());
        }
        Vector3f boundDiag = bound.diagnal();
        int splitAxis = findMaxVector3fComponent(boundDiag);
        if(currentNodeCount == 1){
            //create leaf node and return
            std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr,nullptr,offset,splitAxis,currentNodeCount,bound));
            return newNode;
        }else{
            AABB centroidBound;
            for(int i=begin;i<end;i++){
                centroidBound.unionWith(shapes[i]->bound());
            }
            /*
             * if the maximum component of centroids of the
             * bounds of all shapes are the same,
             * just create a leaf node for all of them
             */

            if(centroidBound.m_min[splitAxis] == centroidBound.m_max[splitAxis]){
                std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr,nullptr,offset,splitAxis,currentNodeCount,bound));
                return newNode;
            }

            //find the split position with minimum cost
            struct bucketInfo{
                std::vector<int> indices;
                AABB bound;
            };
            constexpr int nBuckets = 12;
            bucketInfo buckets[nBuckets];
            for(int i=begin;i<end;i++){
                Point3f center = shapes[i]->bound().center();
                int bucketIndex = nBuckets*(center[splitAxis]/boundDiag[splitAxis]);
                if(bucketIndex == nBuckets)bucketIndex-=1;
                buckets[bucketIndex].indices.push_back(i);
                buckets[bucketIndex].bound.unionWith(shapes[i]->bound());
            }
            Float cost[nBuckets -1];
            for(int i=0;i<nBuckets-1;i++){
                AABB bLeft,bRight;
                int nLeft=0,nRight =0;
                for(int j =0;j<=i;j++){
                    bLeft.unionWith(buckets[j].bound);
                    nLeft += buckets[j].indices.size();
                }
                for(int j=i+1;j<nBuckets;j++){
                    bRight.unionWith(buckets[j].bound);
                    nRight += buckets[j].indices.size();
                }
                cost[i] = 0.125+(nLeft*bLeft.surfaceArea()+
                        bRight.surfaceArea()*nRight)/bound.surfaceArea();
            }
            auto efficientIter = std::min_element(cost,cost+nBuckets-1);
            int efficentIndex = std::distance(cost,efficientIter);
            if(currentNodeCount > 255 || *efficientIter < currentNodeCount){
                //if the count of left nodes is greater than the max count of a leaf node can hold
                //or the cost of spliting the current node is less than creating a leaf node's with current nodes
                auto lambda =[&](const std::shared_ptr<Shape> s){
                    int b = nBuckets*(s->bound().center()[splitAxis]/boundDiag[splitAxis]);
                    if(b == nBuckets)b-=1;
                    return b<efficentIndex;
                };
                int mid = std::distance(shapes.begin(),std::partition(shapes.begin(),shapes.end(),lambda));
                //create interior node
                std::unique_ptr<BVHNode> newNode(
                            new BVHNode(recursiveBuild(shapes,begin,mid,orderedShapes),
                                        recursiveBuild(shapes,mid+1,end,orderedShapes),
                                        offset,splitAxis,currentNodeCount,bound));
                return newNode;

            }else{
                //create leaf node and return
                std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr,nullptr,offset,splitAxis,currentNodeCount,bound));
                return newNode;
            }
        }
    }
};



/*
 *Qt GUI
*/

class QPushButton;
class QLabel;
class QLineEdit;
class QSlider;
class ObjReader;
class QTextEdit;
class OpenGLWidget;

class PathTracingDemo :public BaseDemoWidget
{
    Q_OBJECT
public:
    explicit PathTracingDemo(QWidget * parent = nullptr);
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
