#include "PathTracingDemo.h"
#include "model.h"
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QImage>

#include <QSlider>
#include <QFileDialog>
#include <QMessageBox>

PathTracingDemo::PathTracingDemo(QWidget * parent) :BaseDemoWidget(parent)
{
    QWidget * controlWidget = new QWidget(this);
    QGridLayout * controlLayout = new QGridLayout;

    //button
    m_openFileButton = new QPushButton;
    m_openFileButton->setText(QString("Open..."));
    //lineedit
    m_fileNamesLineEdit = new QLineEdit;
    //label
    m_fileNameLabel = new QLabel;
    m_fileNameLabel->setText(tr("File:"));
    m_fileNamesLineEdit->setReadOnly(true);

    controlLayout->addWidget(m_fileNameLabel, 0, 0, 1, 1, Qt::AlignRight);
    controlLayout->addWidget(m_fileNamesLineEdit, 0, 1);
    controlLayout->addWidget(m_openFileButton, 0, 2);

    //slider label
    m_sliderLabel = new QLabel(tr("Samples:"));
    //slider
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setMaximum(10);
    m_slider->setEnabled(false);
    controlLayout->addWidget(m_sliderLabel, 1, 0, 1, 1, Qt::AlignRight);
    controlLayout->addWidget(m_slider, 1, 1, 1, 2);

    //text edit
    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);
    controlLayout->addWidget(m_textEdit, 2, 0, 1, 3);


    controlWidget->setLayout(controlLayout);
    setControlWidget(controlWidget);

    //create display widget
    m_displayWidget = new QWidget(this);

    m_sceneDisplay = new OpenGLWidget(Camera(), this);
    m_sceneDisplay->setAnimation(true);

    QSize displaySize = m_sceneDisplay->size();
    //m_resultDisplay = new QLabel(this);
    //m_resultDisplay->resize(displaySize);
    //m_resultDisplay->setPixmap(QPixmap::fromImage(QImage()));

    QHBoxLayout * displayLayout = new QHBoxLayout(this);
    displayLayout->addWidget(m_sceneDisplay);
    //displayLayout->addWidget(m_resultDisplay);
    m_displayWidget->setLayout(displayLayout);
    setDisplayWidget(m_displayWidget);

    //signals and slots
    connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSamplesCountChanged(int)));
}


PathTracingDemo::~PathTracingDemo()
{

}

void PathTracingDemo::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("."), QString(".obj(*.obj)"));
    if (fileName.isEmpty() == true)
        return;
    m_fileNamesLineEdit->setText(fileName);
    m_slider->setEnabled(true);
    ObjReader model(fileName.toStdString());
    if (model.isLoaded() == false)
        return;
    //Point3f vertices[] = {
    //    {-0.5f,0.f,0.f},
    //{0.5f,0.f,0.f },
    //{0.f,0.5f,0.f}
    //};
    //unsigned int faceIndices[] = { 0,1,2 };
    //Point3f normals[] = { {0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f} };
    QVector<QVector3D> a =
    {
    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f },
    {0.5f,  0.5f, -0.5f},
    {0.5f,  0.5f, -0.5f} ,
    {-0.5f,  0.5f, -0.5f} ,
    {-0.5f, -0.5f, -0.5f} ,
    {-0.5f, -0.5f,  0.5f},
    {0.5f, -0.5f,  0.5f},
    {0.5f,  0.5f,  0.5f},
    {0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {0.5f,  0.5f,  0.5f},
    {0.5f,  0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f} ,
    {0.5f, -0.5f, -0.5f} ,
    {0.5f, -0.5f,  0.5f} ,
    {0.5f,  0.5f,  0.5f} ,
    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f,  0.5f},
    {0.5f, -0.5f,  0.5f} ,
    {-0.5f, -0.5f,  0.5f} ,
    {-0.5f, -0.5f, -0.5f}  ,
    {-0.5f,  0.5f, -0.5f}  ,
    {0.5f,  0.5f, -0.5f}  ,
    {0.5f,  0.5f,  0.5f}  ,
    {0.5f,  0.5f,  0.5f} ,
    {-0.5f,  0.5f,  0.5f} ,
    {-0.5f,  0.5f, -0.5f}  ,
    };
    QVector<QVector3D> b = {
           {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f, -1.0f},
    {0.0f,  0.0f,  1.0f},
    {0.0f,  0.0f,  1.0f},
    {0.0f,  0.0f,  1.0f},
    {0.0f,  0.0f,  1.0f},
    {0.0f,  0.0f,  1.0f},
    {0.0f,  0.0f,  1.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {1.0f,  0.0f,  0.0f},
    {0.0f, -1.0f,  0.0f},
    {0.0f, -1.0f,  0.0f},
    {0.0f, -1.0f,  0.0f},
    {0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    {0.0f, -1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f},
    {0.0f,  1.0f,  0.0f}
    };
    TriangleMesh mesh(model.getVerticesFlatArray(),
        model.getNormalsFlatArray(),
        model.getVertexCount(), 
        model.getFacesIndicesFlatArray(),
        model.getFacesCount(),
        Trans3DMat());
    m_sceneDisplay->setTriangleMesh(mesh.getVerticesArray(),
        mesh.getNormalsArray(),
        mesh.getVertexCount(),
        (unsigned int*)mesh.getIndicesArray(),
        mesh.getIndexCount());
    //m_sceneDisplay->updateModel(a,b);
}

void PathTracingDemo::onSamplesCountChanged(int value)
{
    qDebug() << "slot hitted";
}
