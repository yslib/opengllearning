#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "openglwindow.h"
int main(int argc, char *argv[])
{
    //For macos ,the format setting must be added before gui is created
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);


    QApplication a(argc,argv);
    MainWindow mainwindow;
	mainwindow.show();
    return a.exec();
}
