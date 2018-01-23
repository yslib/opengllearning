#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

class OpenGLWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget * parent = nullptr);
    ~OpenGLWidget();
protected:
    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;


};

#endif // OPENGLWINDOW_H
