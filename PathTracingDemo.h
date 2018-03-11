#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>


typedef QVector3D Point3f;

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

class AABB3
{
    Point3f m_min;
    Point3f m_max;
public:
    AABB3(const Point3f & p0 = Point3f(),const Point3f & p1 = Point3f()):m_min(p0),m_max(p1){}
    bool intersect(const Ray & ray, Float * t)
    {
        
    }
};

class Shape
{
    AABB3 m_aabb;
public:

};


class Scene
{
    
};



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