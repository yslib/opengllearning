#include <QLayout>
#include <QString>
#include <marchingcubesdemo.h>
#include <QStackedWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglwindow.h"
#include "loopsubdivisionsurfacedemo.h"
#include "PathTracingDemo.h"
#include "clothmodelingdemo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
m_mcDemo(nullptr),
m_loopSubdivisionDemo(nullptr),
m_pathTracingDemo(nullptr),
m_clothModelingDemo(nullptr)
{
    //set main window size
    setMinimumSize(1000, 700);
    setWindowTitle(QString("Graphics Demos"));

    m_stackedWidget = new QStackedWidget;
    //create menu
    m_modeMenu = menuBar()->addMenu(tr("Demos"));
    m_marchingCubesDemoAction = m_modeMenu->addAction(tr("MarchingCubes"));
    m_loopSubdivisionDemoAction = m_modeMenu->addAction(tr("LoopSubdivision"));
    m_pathTracingDemoAction = m_modeMenu->addAction(tr("Path Tracing"));
	m_clothModelingDemoAction = m_modeMenu->addAction(QStringLiteral("Cloth Modeling"));

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
    }else if(action == m_clothModelingDemoAction)
    {
	    if(m_clothModelingDemo == nullptr)
	    {
			m_clothModelingDemo = new ClothModelingDemo(this);
			m_stackedWidget->addWidget(m_clothModelingDemo);
	    }
		m_stackedWidget->setCurrentWidget(m_clothModelingDemo);
    }
}
MainWindow::~MainWindow() {
}
