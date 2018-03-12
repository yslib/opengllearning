#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>


typedef QVector3D Point3f;

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
    QVector3D operator()(float t) { return m_o + t * m_d; }
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
    AABB(const Point3f & p0 = Point3f(),const Point3f & p1 = Point3f()):m_min(p0),m_max(p1){}
    AABB(const Point3f & p):m_min(p),m_max(p){}
    Point3f corner(int i){

    }

    bool intersect(const Ray & ray, Float * t)
    {
        
    }


    static AABB intersect(const AABB & b0,const AABB & b1){

    }
    static AABB unionWith(const AABB & b,const Point3f & p){

    }

    static bool inside(const AABB & b,const Point3f & p){

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
