#ifndef LOOPSUBDIVISIONSURFACEDEMO_H
#define LOOPSUBDIVISIONSURFACEDEMO_H

#include <QWidget>
#include "basedemowidget.h"
#include "openglwindow.h"

#include "core.h"


class QPushButton;
class QLabel;
class QLineEdit;

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

private:
	void LoopSubdivision(const std::vector<int> & vertexIndices,const std::vector<Point3Df> & vertices);
private slots:
	//void onOpenFile();
};

#endif // LOOPSUBDIVISIONSURFACEDEMO_H