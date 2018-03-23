#include "PathTracingDemo.h"
#include "model.h"
#include "framebuffer.h"
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
    m_openFileButton->setText(QString("Open Obj..."));

    m_openMtlFileButton = new QPushButton(tr("Open Mtl.."));


    //lineedit
    m_fileNamesLineEdit = new QLineEdit;
    m_fileNamesLineEdit->setReadOnly(true);
    m_mtlFileNameLineEdit = new QLineEdit;
    m_mtlFileNameLineEdit->setReadOnly(true);

    //label
    m_fileNameLabel = new QLabel;
    m_fileNameLabel->setText(tr("Obj File:"));
    m_mtlNameLabel = new QLabel(tr("Mtl File:"));

    controlLayout->addWidget(m_fileNameLabel, 0, 0, 1, 1, Qt::AlignRight);
    controlLayout->addWidget(m_fileNamesLineEdit, 0, 1);
    controlLayout->addWidget(m_openFileButton, 0, 2);

    controlLayout->addWidget(m_mtlNameLabel, 1, 0, 1, 1, Qt::AlignRight);
    controlLayout->addWidget(m_mtlFileNameLineEdit, 1, 1);
    controlLayout->addWidget(m_openMtlFileButton, 1, 2);

    //slider label
    m_sliderLabel = new QLabel(tr("Samples:"));
    //slider
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setMaximum(10);
    m_slider->setEnabled(false);
    controlLayout->addWidget(m_sliderLabel, 2, 0, 1, 1, Qt::AlignRight);
    controlLayout->addWidget(m_slider, 2, 1, 1, 2);


    m_renderButton = new QPushButton(tr("Render"));
    connect(m_renderButton,SIGNAL(clicked()),this,SLOT(onRender()));
    controlLayout->addWidget(m_renderButton,3,0,1,3);

    //text edit
    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);
    controlLayout->addWidget(m_textEdit, 4, 0, 1, 3);


    controlWidget->setLayout(controlLayout);
    setControlWidget(controlWidget);

    //create display widget
    m_displayWidget = new QWidget(this);

    m_sceneDisplay = new OpenGLWidget(Camera(), this);
    m_sceneDisplay->setAnimation(true);

    QSize displaySize = m_sceneDisplay->size();
    m_resultDisplay = new QLabel(this);
    //m_resultDisplay->resize(displaySize);
    //m_resultDisplay->setPixmap(QPixmap::fromImage(QImage()));

    QHBoxLayout * displayLayout = new QHBoxLayout(this);
    displayLayout->addWidget(m_sceneDisplay);
    displayLayout->addWidget(m_resultDisplay);
    m_displayWidget->setLayout(displayLayout);
    setDisplayWidget(m_displayWidget);

    //signals and slots
    connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenObjectFile()));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSamplesCountChanged(int)));
    connect(m_openMtlFileButton, SIGNAL(clicked()), this, SLOT(onOpenMtlFile()));
}


PathTracingDemo::~PathTracingDemo()
{

}

void PathTracingDemo::onOpenObjectFile()
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
    std::vector<std::shared_ptr<Shape>> triangles = Triangle::createTriangleMesh(model.getVerticesFlatArray(),
        model.getNormalsFlatArray(),
        model.getVertexCount(),
        model.getFacesIndicesFlatArray(),
        model.getFacesCount(),
        model.getIndexToMtlName(),
        m_materialReader,Trans3DMat());
    m_aggregate = std::make_shared<BVHTreeAccelerator>(triangles);
}

void PathTracingDemo::onOpenMtlFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("."), QString(".mtl(*.mtl)"));
    if (fileName.isEmpty() == true)
        return;
    m_mtlFileNameLineEdit->setText(fileName);
    if (m_materialReader.loadFromFile(fileName.toStdString()) == false)
    {
        QMessageBox::critical(this, QString("Error"), QString("can not load mtl file"));
    }


}

void PathTracingDemo::onSamplesCountChanged(int value)
{
    qDebug() << "slot hitted";
}

void PathTracingDemo::onRender()
{
    QSize size = m_sceneDisplay->size();
    m_frameBuffer.resize(size.width(),size.height());
    const Camera & cam = m_sceneDisplay->getCamera();

    Vector3f cameraPosition = cam.position();
    Vector3f cameraFront = cam.front().normalized();
    Vector3f cameraUp = cam.up().normalized();
    Vector3f cameraRight = cam.right().normalized();
    Float distCamToCan = 2;

    Point3f canvasCentroid = cameraPosition+distCamToCan*cameraFront;
    Float canvasHeight = 2*distCamToCan*std::tan(qDegreesToRadians(m_sceneDisplay->verticalAngle()/2));
    Float canvasWidth = canvasHeight*m_sceneDisplay->aspectRatio();
    Point3f canvasTopLeft = canvasCentroid+canvasHeight/2*cameraUp-canvasWidth/2*cameraRight;

    int height = m_frameBuffer.height();
    int width = m_frameBuffer.width();
    Scene scene(m_aggregate);

    QImage resultImage(width, height, QImage::Format_RGB888);
    QColor color(50, 100, 150);
    for(int j=0;j<height;j++){
        for(int i=0;i<width;i++){
            Point3f canvasPosInWorld = canvasTopLeft-cameraUp*(Float(j)/height)*canvasHeight +
                    cameraRight*(Float(i)/width)*canvasWidth;
            //construct a ray
            Ray ray((canvasPosInWorld-cameraPosition).normalized(),cameraPosition);
            Float t;
            Interaction isect;
            if(scene.intersect(ray,&t,&isect) == true){
                Color c(0.3,0.6,0.9);
//                if(isect.bsdf() != nullptr)
//                {
//                    c[0] = 0.1; c[1] = 0.3; c[2] = 0.5;
//                }else
//                {
//                   c = isect.bsdf()->sampleF(Vector3f(), nullptr, nullptr);
//                }
                resultImage.setPixelColor(i, j,QColor(c[0]*256,c[1]*256,c[2]*256));
            }else{
                resultImage.setPixelColor(i, j, QColor(0, 0, 0));
            }
        }
    }
    m_resultDisplay->resize(size);
    m_resultDisplay->setPixmap(QPixmap::fromImage(resultImage));
}
