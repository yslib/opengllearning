#include "PathTracingDemo.h"
#include "model.h"
#include "framebuffer.h"
#include "interaction.h"
#include "light.h"

#include <random>
#include <ctime>

#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QImage>

#include <QSlider>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>

#define DIRECT_ILLUMINATION
#define GI

QDebug operator<<(QDebug d, const Interaction & isect) {
    d << "Interaction:" << "p:" << isect.intersectionPoint() << " norm:" <<
        isect.normal() << " pShape:" << isect.object() << " bsdf:" << isect.bsdf().get();
    return d;
}



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
    //subpixel
    m_subpixelLabel = new QLabel(tr("Subpixel samples:"));
    m_subpixelSpinBox = new QSpinBox(this);
    m_subpixelSpinBox->setMaximum(2000);
    m_subpixelSpinBox->setMinimum(1);
    m_subpixelSpinBox->setValue(800);

    controlLayout->addWidget(m_subpixelLabel,2,0,1,1,Qt::AlignRight);
    controlLayout->addWidget(m_subpixelSpinBox,2,1,1,2);

    //depth
    m_depthLabel = new QLabel(tr("Depth:"));
    m_depthSpinBox = new QSpinBox(this);
    m_depthSpinBox->setMaximum(20);
    m_depthSpinBox->setMinimum(1);
    m_depthSpinBox->setValue(5);
    controlLayout->addWidget(m_depthLabel,3,0,1,1,Qt::AlignRight);
    controlLayout->addWidget(m_depthSpinBox,3,1,1,2);

    //save path
    //m_saveLabel = new QLabel(tr("Save:"));
    m_pathLineEdit= new QLineEdit(this);
    m_pathLineEdit->setReadOnly(true);
    m_pathButton = new QPushButton(tr("Path"));
    m_saveButton = new QPushButton(tr("Save"));

    //controlLayout->addWidget(m_saveButton,4,0,1,1,Qt::AlignRight);
    controlLayout->addWidget(m_pathLineEdit,4,0);
    controlLayout->addWidget(m_pathButton,4,1);
    controlLayout->addWidget(m_saveButton,4,2);

    connect(m_saveButton,SIGNAL(clicked()),this,SLOT(onSaveButton()));
    connect(m_pathButton,SIGNAL(clicked()),this,SLOT(onSavePathButton()));

    //render button
    m_renderButton = new QPushButton(tr("Render"));
    connect(m_renderButton, SIGNAL(clicked()), this, SLOT(onRender()));
    controlLayout->addWidget(m_renderButton, 5, 0, 1, 3);

    //text edit
    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);
    controlLayout->addWidget(m_textEdit, 6, 0, 1, 3);
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

    m_resultImage = QImage(m_resultDisplay->size(),QImage::Format_RGB888);

    //signals and slots
    connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenObjectFile()));
    //connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSamplesCountChanged(int)));
    connect(m_openMtlFileButton, SIGNAL(clicked()), this, SLOT(onOpenMtlFile()));
}


PathTracingDemo::~PathTracingDemo()
{

}

void PathTracingDemo::onSaveButton()
{
    QString path = m_pathLineEdit->text();
    if(path.isEmpty() == true){
        QMessageBox::critical(this,tr("Error"),tr("Path is empty."),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    if(m_resultImage.save(path) == false){
        QMessageBox::critical(this,tr("Error"),tr("Saving failed."),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

}

void PathTracingDemo::onSavePathButton()
{
    QString path = QFileDialog::getSaveFileName(this,QString("Save File"),QString("~/"),QString(".png(*.png)"));
    m_pathLineEdit->setText(path);
}

void PathTracingDemo::onOpenObjectFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("/home/ysl/Downloads/test"), QString(".obj(*.obj)"));
    if (fileName.isEmpty() == true)
        return;
    m_fileNamesLineEdit->setText(fileName);
    //m_slider->setEnabled(true);
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
    std::vector<std::shared_ptr<Shape>> lightShapes;
    std::vector<std::shared_ptr<Shape>> triangles = Triangle::createTriangleMesh(model.getVerticesFlatArray(),
        model.getNormalsFlatArray(),
        model.getVertexCount(),
        model.getFacesIndicesFlatArray(),
        model.getFacesCount(),
        model.getIndexToMtlName(),
        m_materialReader,&lightShapes, Trans3DMat());
    //build bvh accelerator
    m_aggregate = std::make_shared<BVHTreeAccelerator>(triangles);
    //buid lights
    for(const auto & light:lightShapes){
        m_lights.push_back(std::make_shared<AreaLight>(light));
    }
}

void PathTracingDemo::onOpenMtlFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("/home/ysl/Downloads/test"), QString(".mtl(*.mtl)"));
    if (fileName.isEmpty() == true)
        return;
    m_mtlFileNameLineEdit->setText(fileName);
    qDebug()<<fileName;
    if (m_materialReader.loadFromFile(fileName.toStdString()) == false)
    {
        QMessageBox::critical(this, QString("Error"), QString("can not load mtl file"));
    }


}

void PathTracingDemo::onSamplesCountChanged(int value)
{
    qDebug()<<value;
}

void PathTracingDemo::onDepthChanged(int depth)
{
    qDebug()<<depth;
}


//global random engine
std::default_random_engine e(time(0));
std::uniform_real_distribution<Float> u(Float(0), Float(1));


Color trace(const Scene & scene,
    const Ray & ray,
    int depth) {
    //qDebug() << "depth:" << depth;
    Color directIllumination(0,0,0);
    Float t;
    Interaction isect;
    Color indirectIllumination(0, 0, 0);

    if (scene.intersect(ray, &t, &isect) == false) {
        return Color(0, 0, 0);
    }
    Color emission = isect.bsdf()->emmision();
    //qDebug()<<emission;
    //Color emission = Color(0,0,0);
       // assert(isect.object() != nullptr);
        if (depth == 0) {
            //return material emission
            return emission;
        }
        const std::shared_ptr<Material> & m = isect.object()->getMaterial();
#ifdef DIRECT_ILLUMINATION
        const auto & lights = scene.lights();
        int lightCount = lights.size();
        int lightSamples = 1;
        //direct light illumination
        for (const auto & light : lights) {
            Vector3f wi;
            Float pdf;
            VisibilityTester vis;

            for(int i=0;i<lightSamples;i++){

                Point2f sample(u(e), u(e));
                light->sampleLi(isect, &wi, &pdf, uniformSampleTriangle(sample), &vis);

                if (vis.occlude(scene) == false)
                {
                    Color li = light->L(isect, wi);
                    Float cosTheta = Vector3f::dotProduct(vis.to().normal().normalized(),-wi.normalized());
                    li = li*cosTheta/vis.distanceSquare()*100;
                    //qDebug()<<"visible to light"<<li;
                    if (m != nullptr) {
                        Color ks = m->m_ks;
                        Color ka = m->m_ka;
                        Color kd = m->m_kd;
                        Vector3f n = isect.normal().normalized();
                        Vector3f l = wi.normalized();
                        Vector3f h = (-ray.direction() - wi).normalized();
                   switch (m->m_type)
                    {
                    case MaterialType::Mirror:
                        //directIllumination += ka * li + kd * (std::max(Vector3f::dotProduct(n, l), 0.0f))*li + ks * (std::max(Vector3f::dotProduct(n, h), 0.0f))*li;
                        //directIllumination += ka*li*0.01;
                        break;
                    case MaterialType::Metal:
                        directIllumination += ka * li + kd * (std::max(Vector3f::dotProduct(n, l), 0.0f))*li + ks * (std::max(Vector3f::dotProduct(n, h), 0.0f))*li;
                        break;
                    case MaterialType::Glass:
                        //directIllumination += ka * li + kd * (std::max(Vector3f::dotProduct(n, l), 0.0f))*li + ks * (std::max(Vector3f::dotProduct(n, h), 0.0f))*li;

                        break;
                    default:
                        assert(false);
                        break;
                    }
//                        directIllumination += ka * li + kd * (std::max(Vector3f::dotProduct(n, l), 0.0f))*li + ks * (std::max(Vector3f::dotProduct(n, h), 0.0f))*li;
                    }
                }
            }
        }

        directIllumination = directIllumination/static_cast<Float>(lightSamples*lightCount);
#endif
        //indirect light illumination
#ifdef GI
        Color bsdf;
        Vector3f wi;
        Float pdf;
        Point2f sample(u(e), u(e));
        //qDebug() << sample;
        //qDebug() << (m == nullptr ? false : true);
        switch (m->m_type)
        {
        case MaterialType::Mirror:
            bsdf = isect.bsdf()->sampleF(-ray.direction(), &wi, &pdf, sample, BSDF_SPECULAR);
            //qDebug() << "mirror" << bsdf << " " << wi;
            break;
        case MaterialType::Metal:
            bsdf = isect.bsdf()->sampleF(-ray.direction(), &wi, &pdf, sample, BSDF_DIFFUSE);
            //qDebug() << "metal" << bsdf << " " << wi;
            break;
        case MaterialType::Glass:
            bsdf = isect.bsdf()->sampleF(-ray.direction(), &wi, &pdf, sample, BSDF_REFRACTION);
            //qDebug() << "glass" << bsdf;
            break;
        default:
            assert(false);
            break;
        }
        Ray newRay = isect.spawnRay(wi);
        indirectIllumination = bsdf * trace(scene,newRay,depth-1);
#endif
        return (emission+directIllumination + indirectIllumination);

}

void PathTracingDemo::onRender()
{
    QSize size = m_sceneDisplay->size();
    m_frameBuffer.resize(size.width(), size.height());
    const Camera & cam = m_sceneDisplay->getCamera();

    Vector3f cameraPosition = cam.position();
    Vector3f cameraFront = cam.front().normalized();
    Vector3f cameraUp = cam.up().normalized();
    Vector3f cameraRight = cam.right().normalized();
    Float distCamToCan = 2;

    Point3f canvasCentroid = cameraPosition + distCamToCan * cameraFront;
    Float canvasHeight = 2 * distCamToCan*std::tan(qDegreesToRadians(m_sceneDisplay->verticalAngle() / 2));
    Float canvasWidth = canvasHeight * m_sceneDisplay->aspectRatio();
    Point3f canvasTopLeft = canvasCentroid + canvasHeight / 2 * cameraUp - canvasWidth / 2 * cameraRight;

    int height = m_frameBuffer.height();
    int width = m_frameBuffer.width();

    Float pixelLength = canvasWidth / static_cast<Float>(width);

    //Test Light
    const Point3f vertices[6] =
    { { -2.5f,4.6f,7.5f },
    { 2.5f,4.6f,7.5f },
    { 2.5f,4.6f,2.5f },
    { -2.5f,4.6f,7.5f },
    { 2.5f,4.6f,2.5f },
    { -2.5f,4.6f,2.5f }
    };
    const Vector3f normals[6] = {
        { 0,-1,0 } ,
    { 0,-1,0 },
    { 0,-1,0 } ,
    { 0,-1,0 } ,
    { 0,-1,0 } ,
    { 0,-1,0 } };
    const int indices[6] = { 0,1,2,3,4,5 };
    std::unordered_map<int, std::string> tempMtl;
    MaterialReader rd;
    std::vector<std::shared_ptr<Shape>> lightShape =
        Triangle::createTriangleMesh(vertices,
            normals, 6, indices, 6,
            tempMtl,
            rd, nullptr,Trans3DMat());

    std::shared_ptr<AreaLight> aTestLight = std::make_shared<AreaLight>(lightShape[0], Color(255,255,255));
    std::shared_ptr<AreaLight> bTestLight = std::make_shared<AreaLight>(lightShape[1], Color(255.0, 255.0, 255.0));
    std::vector<std::shared_ptr<AreaLight>> lights;
    lights.push_back(aTestLight);
    lights.push_back(bTestLight);
    Scene scene(m_aggregate,m_lights);
    //result output
    m_resultImage = QImage(width, height, QImage::Format_RGB888);
    //random number
    int maxDepth = m_depthSpinBox->value();
    int subpixels = m_subpixelSpinBox->value();

    int totalPixels = height*width*subpixels;
    int interval = totalPixels/100;
    int process = 0;

    qDebug()<<"max depth:"<<maxDepth<<" subpixel samples:"<<subpixels;
    for (int j = 0; j < height; j++) {
//#pragma omp parallel for
        for (int i = 0; i < width; i++) {
                      Color L(0.0, 0.0, 0.0);
            Point3f canvasPosInWorld = canvasTopLeft - cameraUp * (Float(j) / height)*canvasHeight +
                cameraRight * (Float(i) / width)*canvasWidth;
            //construct a ray
            for (int k = 0; k < subpixels; k++) {
                Point2f p = Point2f(u(e), u(e)) * 2 - Point2f(1, 1);
                Ray ray((canvasPosInWorld + p[0]*cameraRight*pixelLength+p[1]*cameraUp*pixelLength- cameraPosition).normalized(), cameraPosition);
                L += trace(scene, ray, maxDepth);
            }
            L = clamp(L / (subpixels), Color(0, 0, 0), Color(255, 255, 255));
            m_resultImage.setPixelColor(i, j, QColor(L[0], L[1], L[2]));
            if((process++)%interval == 0){
                qreal p = (qreal)process/totalPixels;
                QString pt = QString::number(p)+"%";
                qDebug()<<p;
                //m_textEdit->setText(pt);
            }
        }

    }
    m_resultDisplay->resize(size);
    m_resultDisplay->setPixmap(QPixmap::fromImage(m_resultImage));
}
