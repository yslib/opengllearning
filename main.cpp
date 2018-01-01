#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "openglwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    OpenGLWidget window;
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3,2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    window.setFormat(fmt);
    window.show();
    return a.exec();
}