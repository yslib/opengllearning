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
#include "loopsubdivisionsurfacedemo.h"
#include "PathTracingDemo.h"
#include <QMenu>
#include <QAction>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_mcDemo(nullptr), m_loopSubdivisionDemo(nullptr), m_pathTracingDemo(nullptr)
{
    //set main window size
    setMinimumSize(1000, 700);
    setWindowTitle(QString("Graphics Demos"));

    m_stackedWidget = new QStackedWidget;
    //create menu
    m_modeMenu = menuBar()->addMenu(tr("Mode"));
    m_marchingCubesDemoAction = m_modeMenu->addAction(tr("MarchingCubes"));
    m_loopSubdivisionDemoAction = m_modeMenu->addAction(tr("LoopSubdivision"));

    m_pathTracingDemoAction = m_modeMenu->addAction(tr("Path Tracing"));

    connect(m_modeMenu, &QMenu::triggered, this, &MainWindow::onMenuActions);

    setCentralWidget(m_stackedWidget);
}



void MainWindow::setWidget(BaseDemoWidget *widget)
{
    //m_mainLayout->addWidget(new QWidget(this),0,1);
    //m_mainLayout->addWidget(new OpenGLWidget(this),0,0);
    m_mainLayout->addWidget(widget->displayWidget(), 1, 0);
    m_mainLayout->addWidget(widget->controlWidget(), 1, 1);
}

void MainWindow::onMenuActions(QAction * action)
{

    if (action == m_marchingCubesDemoAction) {
        if (m_mcDemo == nullptr) {
            m_mcDemo = new MarchingCubesDemo(this);
            m_stackedWidget->addWidget(m_mcDemo);
        }
        m_stackedWidget->setCurrentWidget(m_mcDemo);
    }
    else if (action == m_loopSubdivisionDemoAction) {
        if (m_loopSubdivisionDemo == nullptr) {
            m_loopSubdivisionDemo = new LoopSubdivisionSurfaceDemo(this);
            m_stackedWidget->addWidget(m_loopSubdivisionDemo);
        }
        m_stackedWidget->setCurrentWidget(m_loopSubdivisionDemo);
    }else if(action == m_pathTracingDemoAction)
    {
        if(m_pathTracingDemo == nullptr)
        {
            m_pathTracingDemo = new PathTracingDemo(this);
            m_stackedWidget->addWidget(m_pathTracingDemo);
        }
        m_stackedWidget->setCurrentWidget(m_pathTracingDemo);
    }
}
MainWindow::~MainWindow() {
}
