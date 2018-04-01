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
class QSpinBox;

class QCheckBox;


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

    int m_subpixel;

    QPushButton * m_renderButton;

    FrameBuffer m_frameBuffer;
    QImage m_resultImage;
    MaterialReader m_materialReader;

    //subpixel sample control
    QLabel * m_subpixelLabel;
    QSpinBox * m_subpixelSpinBox;

    //recursion depth
    QLabel * m_depthLabel;
    QSpinBox * m_depthSpinBox;

    //
    QPushButton * m_saveButton;

    //direct illumination and gi illumination option
    QCheckBox *m_GIButtion;
    QCheckBox *m_DIButtion;

    QLabel * m_intensityLabel;
    QSpinBox * m_intensitySpinBox;
    //renderer related
    std::unique_ptr<Scene> m_scene;
    std::shared_ptr<BVHTreeAccelerator> m_aggregate;
    std::vector<std::shared_ptr<AreaLight>> m_lights;

    public slots:
    void onSaveButton();
    void onOpenObjectFile();
    void onOpenMtlFile();
    void onSamplesCountChanged(int value);
    void onDepthChanged(int depth);
    void onRender();
};
#endif
