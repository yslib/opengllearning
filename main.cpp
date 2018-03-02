#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "openglwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    MainWindow mainwindow;
	mainwindow.show();
    return a.exec();
}
