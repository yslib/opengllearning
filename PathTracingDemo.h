#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"
#include "openglwindow.h"
#include <QVector3D>

class Ray
{
    QVector3D m_o;
    QVector3D m_d;
    float m_t;
public:
    Ray(const QVector3D & d, const QVector3D & o, float t) :m_o(o), m_d(d), m_t(t) {}
    QVector3D operator()(float t) { return m_o + t * m_d; }
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