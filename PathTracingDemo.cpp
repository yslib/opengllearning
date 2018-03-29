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


QDebug operator<<(QDebug d, const Interaction & isect) {
    d << "Interaction:" << "p:" << isect.intersectionPoint() << " norm:" <<
        isect.normal() << " pShape:" << isect.object() << " bsdf:" << isect.bsdf().get();
    return d;
}



PathTracingDemo::PathTracingDemo(QWidget * parent) :BaseDemoWidget(parent)
{
    QWidget * controlWidget = new QWidget(this);
    QGridLayout * controlLayout = new QGridLayout;

    m_samples = 1;
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
    connect(m_renderButton, SIGNAL(clicked()), this, SLOT(onRender()));
    controlLayout->addWidget(m_renderButton, 3, 0, 1, 3);

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
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("/home/ysl/Desktop/test"), QString(".obj(*.obj)"));
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
        m_materialReader, Trans3DMat());
    m_aggregate = std::make_shared<BVHTreeAccelerator>(triangles);
}

void PathTracingDemo::onOpenMtlFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("/home/ysl/Desktop/test"), QString(".mtl(*.mtl)"));
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
    qDebug() << "slot hitted";
    if(value >40)
        value =40;
    else if(value <1)
        value = 1;
    m_samples = value;

    qDebug()<<value;
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
        assert(isect.object() != nullptr);
        if (depth > 5) {
            //return material emission
            return Color(0, 0, 0);
        }
        const auto & lights = scene.lights();
        const std::shared_ptr<Material> & m = isect.object()->getMaterial();
        assert(m != nullptr);
        //direct light illumination
        for (const auto & light : lights) {
            Vector3f wi;
            Float pdf;
            VisibilityTester vis;
            Point2f sample(u(e), u(e));

            light->sampleLi(isect, &wi, &pdf, uniformSampleTriangle(sample), &vis);

            if (vis.occlude(scene) == false)
            {
                Color li = light->L(isect, wi);

                if (m != nullptr) {
                    Color ks = m->m_kd;
                    Color ka = m->m_ka;
                    Color kd = m->m_kd;
                    Vector3f n = isect.normal().normalized();
                    Vector3f l = wi.normalized();
                    Vector3f h = (-ray.direction() - wi).normalized();
                    directIllumination += ka * li + kd * (std::max(Vector3f::dotProduct(n, l), 0.0f))*li + ks * (std::max(Vector3f::dotProduct(n, h), 0.0f))*li;
                    //qDebug() << ka << " " << kd << " " << ks;
                    //L += li * ka;
                }
                //L = Color(1.0, 1.0, 1.0);
            }
        }
        //indirect light illumination
       
        Color bsdf;
        Vector3f wi;
        Float pdf;
        Point2f sample(u(e), u(e));
        //qDebug() << (m == nullptr ? false : true);
        switch (m->m_type)
        {
        case MaterialType::Mirror:
            bsdf = isect.bsdf()->sampleF(-ray.direction(), &wi, &pdf, sample, BSDF_SPECULAR);
            //qDebug() << "mirror"<<bsdf;
            break;
        case MaterialType::Metal:
            bsdf = isect.bsdf()->sampleF(-ray.direction(), &wi, &pdf, sample, BSDF_DIFFUSE);
            //qDebug() << "metal" << bsdf;
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
        indirectIllumination = bsdf * trace(scene,newRay, depth + 1);
        return (directIllumination + indirectIllumination);

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
            rd, Trans3DMat());

    std::shared_ptr<AreaLight> aTestLight = std::make_shared<AreaLight>(lightShape[0], Color(100,100,100));
    //std::shared_ptr<AreaLight> bTestLight = std::make_shared<AreaLight>(lightShape[1], Color(1.0, 1.0, 1.0));
    std::vector<std::shared_ptr<AreaLight>> lights;
    lights.push_back(aTestLight);
    //lights.push_back(bTestLight);

    Scene scene(m_aggregate,lights);

    //result output
    QImage resultImage(width, height, QImage::Format_RGB888);

    //random number

    int totalPixels = height*width;
    int interval = totalPixels/100;
    int process = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Color L(0.0, 0.0, 0.0);
            Point3f canvasPosInWorld = canvasTopLeft - cameraUp * (Float(j) / height)*canvasHeight +
                cameraRight * (Float(i) / width)*canvasWidth;
            //construct a ray
            Ray ray((canvasPosInWorld - cameraPosition).normalized(), cameraPosition);
            for (int i = 0; i < m_samples; i++)
                L += trace(scene, ray, 0);
            //qDebug() << L;
            //qDebug() << L;
            L = clamp(L/m_samples,Color(0,0,0),Color(255,255,255));
            resultImage.setPixelColor(i, j,QColor(L[0],L[1],L[2]));
//            if((process++)%interval){
//                qreal p = (qreal)process/totalPixels;
//                QString pt = QString::number(p)+"%";
//                qDebug()<<p;
//                //m_textEdit->setText(pt);
//            }
        }
    }
    m_resultDisplay->resize(size);
    m_resultDisplay->setPixmap(QPixmap::fromImage(resultImage));
}
