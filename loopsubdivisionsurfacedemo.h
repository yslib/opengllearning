#ifndef LOOPSUBDIVISIONSURFACEDEMO_H
#define LOOPSUBDIVISIONSURFACEDEMO_H

#include <QWidget>
#include "basedemowidget.h"
#include "openglwindow.h"

#include "core.h"
#include <QVector>
#include <QVector3D>
#include <QPair>
#include "model.h"


class QPushButton;
class QLabel;
class QLineEdit;
class QSlider;
class Model;
class QTextEdit;
class LoopSubdivisionSurfaceDemo:public BaseDemoWidget
{
	Q_OBJECT
public:
    LoopSubdivisionSurfaceDemo(QWidget * parent = nullptr);
private:
	QPushButton * m_openFileButton;
	QLabel * m_label;
	QLineEdit * m_fileNamesLineEdit;
	OpenGLWidget * m_displayWidget;
	QLabel * m_sliderLabel;
	QSlider * m_slider;
	QTextEdit * m_textEdit;
	QScopedPointer<Model> m_model;
private:
	QPair<QVector<QVector3D>,QVector<QVector3D>> LoopSubdivision(const std::vector<int> & vertexIndices,const std::vector<Point3Df> & vertices,int LEVEL = 3);
private slots:
	void onOpenFile();
	void onRecursionsCountChanged(int value);
};

#endif // LOOPSUBDIVISIONSURFACEDEMO_H
