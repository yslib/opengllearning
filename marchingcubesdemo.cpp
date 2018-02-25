#include "marchingcubesdemo.h"
#include "openglwindow.h"
#include <QLabel>
#include <QLayout>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>


constexpr int MarchingCubesDemo::m_triangleTable[256][16];
constexpr int MarchingCubesDemo::m_edgeToVertex[12][6];

MarchingCubesDemo::MarchingCubesDemo(QWidget *parent)noexcept:BaseDemoWidget(parent),m_dataDepth(0),
    m_dataHeight(0),m_dataWidth(0)
{
    //main layout
    m_layout = new QGridLayout(this);
    OpenGLWidget* displayWidget = new OpenGLWidget(this);
    displayWidget->setMinimumHeight(500);
    displayWidget->setMinimumWidth(500);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(3,3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);
    displayWidget->setFormat(fmt);
    displayWidget->setAnimation(true);

    //test model
    QVector<QVector3D> model;

    model<<QVector3D(-0.5f,-0.5f,0.0f)
           <<QVector3D(0.5f,-0.5f,0.0f)
          <<QVector3D(0.0f,0.5f,0.0f);
    displayWidget->updateModel(model);
    //



    setDisplayWidget(displayWidget);
    m_layout->addWidget(displayWidget,0,0);


    //controal widget
    QGridLayout * controlWidgetLayout = new QGridLayout(this);
    QWidget * controlWidget = new QWidget(this);
    controlWidget->setLayout(controlWidgetLayout);
    setControlWidget(controlWidget);

    m_mcFileNameLabel = new QLabel(QString("File Name:"),this);
    m_mcFileNameLineEdit = new QLineEdit(this);
    m_mcFileNameLineEdit->setReadOnly(true);
    m_mcOpenFileButton = new QPushButton(this);
    m_mcOpenFileButton->setText(QString("Open..."));
    controlWidgetLayout->addWidget(m_mcFileNameLabel,0,0);
    controlWidgetLayout->addWidget(m_mcFileNameLineEdit,0,1);
    controlWidgetLayout->addWidget(m_mcOpenFileButton,0,2);


    m_mcISOValueLabel = new QLabel(QString("ISO Value:"));
    m_mcISOValueSlider = new QSlider(Qt::Horizontal,this);
    m_mcISOValueSlider->setMinimum(0);
    m_mcISOValueSlider->setMaximum(255);
    m_mcISOValueSlider->setEnabled(false);
    controlWidgetLayout->addWidget(m_mcISOValueLabel,1,0);
    controlWidgetLayout->addWidget(m_mcISOValueSlider,1,1,1,2);

    m_testButton = new QPushButton(this);
    m_testButton->setText("Change Model");
    controlWidgetLayout->addWidget(m_testButton,2,0,1,3);


    //Connect signals and slots
    connect(m_mcOpenFileButton,SIGNAL(clicked()),this,SLOT(onOpenFile()));
    connect(m_mcISOValueSlider,SIGNAL(valueChanged(int)),this,SLOT(onISOValueChanged(int)));

    connect(m_testButton,SIGNAL(clicked()),this,SLOT(onTestChanged()));
}

MarchingCubesDemo::~MarchingCubesDemo()
{

}

int MarchingCubesDemo::toIndex(int z, int y, int x)
{
    return x+y*m_dataWidth+z*m_dataWidth*m_dataHeight;
}


std::tuple<int,int,int> MarchingCubesDemo::toPoint(int index)
{
    int x = index%m_dataWidth;
    int z = index/(m_dataWidth*m_dataHeight);
    int y = (index - z*m_dataWidth*m_dataHeight)/m_dataWidth;
    return std::tuple<int,int,int>(x,y,z);
}

bool MarchingCubesDemo::greaterThanISO(int index,unsigned char iso)
{
    return m_rawData[index]>iso;
}

QVector<QVector3D> MarchingCubesDemo::MarcingCubes(int value)
{
    QVector<QVector3D> triangles;
    //qDebug()<<m_dataDepth<<" "<<m_dataHeight<<" "<<m_dataWidth;
    for(int z = 0 ;z<m_dataDepth-1;z++){
        for(int y = 0;y<m_dataHeight-1;y++){
            for(int x = 0;x<m_dataWidth-1;x++){
                int id = 0;
                id += greaterThanISO(toIndex(z,y,x),value)?(1<<0):(0);
                id += greaterThanISO(toIndex(z,y,x+1),value)?(1<<1):(0);
                id += greaterThanISO(toIndex(z,y+1,x),value)?(1<<2):(0);
                id += greaterThanISO(toIndex(z,y+1,x+1),value)?(1<<3):(0);
                id += greaterThanISO(toIndex(z+1,y,x),value)?(1<<4):(0);
                id += greaterThanISO(toIndex(z+1,y,z+1),value)?(1<<5):(0);
                id += greaterThanISO(toIndex(z+1,y+1,x),value)?(1<<6):(0);
                id += greaterThanISO(toIndex(z+1,y+1,x+1),value)?(1<<7):(0);
                 for(int i=0;m_triangleTable[id][i] != -1;i+=3){
                     //for a single triangle mesh
                       for(int j=0;j<3;j++){
                           int e = m_triangleTable[id][i+j];
                           int v1x = x+m_edgeToVertex[e][0];
                           int v1y = y+m_edgeToVertex[e][1];
                           int v1z = z+m_edgeToVertex[e][2];
                           int v2x = x+m_edgeToVertex[e][3];
                           int v2y = y+m_edgeToVertex[e][4];
                           int v2z = z+m_edgeToVertex[e][5];

                           int v1value = m_rawData[toIndex(v1z,v1y,v1x)];
                           int v2value = m_rawData[toIndex(v2z,v2y,v2x)];

                           triangles.push_back(interpulation(v1x,v1y,v1z,v2x,v2y,v2z,v1value,v2value,value));

                       }
                 }
            }
        }
    }

    return triangles;
}
QVector3D MarchingCubesDemo::interpulation(int x1, int y1, int z1, int x2, int y2, int z2, int value1, int value2, int iso)
{
   return QVector3D(m_dataXSpace*(static_cast<float>(iso-value1)*(x2-x1)/(value1-value2)+x1),
                    m_dataYSpace*(static_cast<float>(iso-value1)*(y2-y1)/(value1-value2)+y1),
                    m_dataZSpace*(static_cast<float>(iso-value1)*(z2-z1)/(value1-value2)+z1));
}

void MarchingCubesDemo::onOpenFile()
{
    QString path = QFileDialog::getOpenFileName(this,QString(".vifo file"),QString("~/Downloads/"),
                                                QString(".vifo (*.vifo)"));

    //No File Name
    if(path.isEmpty() == true)
        return;


    //File doesn't exist
    QFileInfo fileInfo(path);
    if(fileInfo.exists() == false)
        QMessageBox::critical(this,QString("Error"),QString("File does not exist"),QMessageBox::Ok,QMessageBox::Ok);

    QFile file(path);
    //can not be opened
    if(file.open(QIODevice::ReadOnly|QIODevice::Text) == false)
        QMessageBox::critical(this,QString("Error"),QString("File can not be opened"),QMessageBox::Ok,QMessageBox::Ok);

    QString targetFile;
    QTextStream ts(&file);
    //target file info
    ts>>m_dataWidth>>m_dataHeight>>m_dataDepth;
    ts>>m_dataXSpace>>m_dataYSpace>>m_dataZSpace;
    ts>>targetFile;
    qDebug()<<"TargetFileName:"<<targetFile;
    qDebug()<<m_dataWidth<<" "<<m_dataHeight<<" "<<m_dataDepth;
    qDebug()<<m_dataXSpace<<" "<<m_dataYSpace<<" "<<m_dataZSpace;
    file.close();


    QString fileName = fileInfo.path() +"/"+ targetFile;
    qDebug()<<"TargetPath:"<<fileName;

    QFile dataFile(fileName);
    if(dataFile.open(QIODevice::ReadOnly) == false){
        QMessageBox::critical(this,QString("Error"),QString("File can not be opened"),QMessageBox::Ok,QMessageBox::Ok);
    }
    qint64 length =m_dataWidth*m_dataHeight*m_dataDepth;
    unsigned char * tmp = new unsigned char[length];
    qDebug()<<"FileLength:"<<length;
    qint64 readLength = dataFile.read((char*)(tmp),length);
    if(readLength != length){
        QMessageBox::warning(this,QString("Warning"),
                             tr("Only read %1 byte(s) of %2 byte(s)").arg(readLength).arg(length),
                             QMessageBox::Ok,QMessageBox::Ok);
    }

    //set the raw pointer to the smart pointer
    m_rawData.reset(tmp);
    m_mcISOValueSlider->setEnabled(true);
}
void MarchingCubesDemo::onISOValueChanged(int value)
{
    OpenGLWidget * widget = dynamic_cast<OpenGLWidget *>(displayWidget());
    if(widget != nullptr){
       widget->updateModel(MarcingCubes(value));
    }

}

void MarchingCubesDemo::onTestChanged()
{
    OpenGLWidget * widget = dynamic_cast<OpenGLWidget *>(displayWidget());
    if(widget != nullptr){
        QVector<QVector3D> model;
        model<<QVector3D(-0.5f,-0.5f,0.0f)
           <<QVector3D(0.5f,-0.5f,0.0f)
          <<QVector3D(0.0f,0.5f,0.0f)
         <<QVector3D(-0.5f,0.5f,0.0f)
           <<QVector3D(0.5f,0.5f,0.0f)
          <<QVector3D(0.0f,-0.5f,0.0f);
        widget->updateModel(model);
    }
}
