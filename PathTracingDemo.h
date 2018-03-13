#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>
#include <cassert>

#include <numeric>


typedef QVector3D Point3f;

constexpr Float MIN_Float_VALUE = std::numeric_limits<Float>::lowest();
constexpr Float MAX_Float_VALUE = std::numeric_limits<Float>::max();

/*
 *
*/

class Ray
{
    Point3f m_o;
    Point3f m_d;
    Float m_t;
public:
    Ray(const Point3f & d, const Point3f & o, Float t) :m_o(o), m_d(d), m_t(t) {}
    Point3f operator()(float t) { return m_o + t * m_d; }
    friend class AABB3;
};

/*
 * Axis-aligned Bounding Box
 */



class AABB
{
    Point3f m_min;
    Point3f m_max;
public:
    AABB(const Point3f & p0 = Point3f(MIN_Float_VALUE,MIN_Float_VALUE,MIN_Float_VALUE),
         const Point3f & p1 = Point3f(MAX_Float_VALUE,MAX_Float_VALUE,MAX_Float_VALUE)):
        m_min(std::min(p0.x(),p1.x()),std::min(p0.y(),p1.y()),std::min(p0.z(),p1.z())),
        m_max(std::max(p0.x(),p1.x()),std::max(p0.y(),p1.y()),std::max(p0.z(),p1.z()))
    {}
    AABB(const Point3f & p):m_min(p),m_max(p){}

    const Point3f & operator[](int i)const{
        assert(i>=0&&i<2);
        if(i==0)return m_min;
        if(i==1)return m_max;
        return Point3f();
    }

    Point3f & operator[](int i){
        return const_cast<Point3f>(static_cast<const AABB>(*this)[i]);
    }

    Point3f corner(int i)const{
        return Point3f(
                    (*this)[i&1].x(),(*this)[i&2].y(),(*this)[i&4].z()
                    );
    }

    //whether this intersect with a ray
    bool intersect(const Ray & ray, Float * t = nullptr)const
    {
        
    }
    //a point is inside the bounding box
    bool inside(const Point3f & p)const{

    }

    AABB overlap(const AABB & b)const{

    }
    AABB unionWith(const AABB & b)const{

    }
    AABB unionWith(const Point3f & p)const{

    }
};

class Shape
{
public:
    virtual AABB bound()const=0;
    virtual Float area()const=0;
    virtual bool intersect(const Ray & ray,Float * t)const=0;
};

class TriangleMesh{
    std::unique_ptr<Point3f[]> m_vertices;
    std::unique_ptr<int[]> m_vertexIndices;

public:

    friend class Triangle;

};

class Triangle:public Shape{

public:

};


class Scene
{
    AABB m_worldBound;
    std::shared_ptr<Shape> m_shape;
public:
    bool intersect(const Ray & ray,Float * t){

    }
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
