#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLayout>
#include "openglwindow.h"
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <marchingcubesdemo.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
     m_mainLayout = new QGridLayout(this);
     QWidget * center = new QWidget(this);
     setCentralWidget(center);
     center->setLayout(m_mainLayout);


     //MC Demos
     m_mcDemo = new MarchingCubesDemo(this);

     setWidget(m_mcDemo);
}

void MainWindow::setWidget(BaseDemoWidget *widget)
{
    m_mainLayout->addWidget(widget->displayWidget(),0,0);
    m_mainLayout->addWidget(widget->controlWidget(),0,1);
}

MainWindow::~MainWindow(){
}
