#include "openglwindow.h"
#include <QDebug>


OpenGLWidget::OpenGLWidget(QWidget *parent):QOpenGLWidget(parent){

}
OpenGLWidget::~OpenGLWidget()
{

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0,0,0,0);

}
void OpenGLWidget::resizeGL(int w, int h){

}
void OpenGLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);

}
