#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include <QVector3D>
#include <cassert>
#include <limits>
#include <algorithm>
#include <iterator>
#include <unordered_map>


#include "basedemowidget.h"
#include "openglwindow.h"
#include "framebuffer.h"
#include "utils.h"
#include "model.h"
#include "geometry.h"
#include "shape.h"
#include "scene.h"
#include "bvh.h"

/*
 * Axis-aligned Bounding Box
 */

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

    QPushButton * m_openMtlFileButton;
    QLabel * m_mtlNameLabel;
    QLineEdit *m_mtlFileNameLineEdit;

    QWidget * m_displayWidget;

    OpenGLWidget * m_sceneDisplay;
    QLabel * m_resultDisplay;

    QLabel * m_sliderLabel;
    QSlider * m_slider;
    QTextEdit * m_textEdit;

    QPushButton * m_renderButton;

    FrameBuffer m_frameBuffer;
    QImage m_image;

    MaterialReader m_materialReader;
    //
    std::unique_ptr<Scene> m_scene;
    std::shared_ptr<BVHTreeAccelerator> m_aggregate;

    public slots:
    void onOpenObjectFile();
    void onOpenMtlFile();
    void onSamplesCountChanged(int value);
    void onRender();
};
#endif
