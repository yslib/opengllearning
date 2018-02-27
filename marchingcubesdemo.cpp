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

MarchingCubesDemo::MarchingCubesDemo(QWidget *parent)noexcept:BaseDemoWidget(parent), m_dataDepth(0),
m_dataHeight(0), m_dataWidth(0)
{
	//main layout
	m_layout = new QGridLayout(this);
	OpenGLWidget* displayWidget = new OpenGLWidget(this);


	QSurfaceFormat fmt;
	fmt.setDepthBufferSize(24);
	fmt.setStencilBufferSize(8);
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(fmt);
	displayWidget->setFormat(fmt);
	displayWidget->setAnimation(true);

	//test model
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
	displayWidget->updateModel(a,b);
	//
	setDisplayWidget(displayWidget);
    //m_layout->addWidget(displayWidget, 0, 0);


	//controal widget
	QGridLayout * controlWidgetLayout = new QGridLayout(this);
	QWidget * controlWidget = new QWidget(this);
	controlWidget->setLayout(controlWidgetLayout);
	setControlWidget(controlWidget);

	m_mcFileNameLabel = new QLabel(QString("File Name:"), this);
	m_mcFileNameLineEdit = new QLineEdit(this);
	m_mcFileNameLineEdit->setReadOnly(true);
	m_mcOpenFileButton = new QPushButton(this);
	m_mcOpenFileButton->setText(QString("Open..."));
	controlWidgetLayout->addWidget(m_mcFileNameLabel, 0, 0);
	controlWidgetLayout->addWidget(m_mcFileNameLineEdit, 0, 1);
	controlWidgetLayout->addWidget(m_mcOpenFileButton, 0, 2);


	m_mcISOValueLabel = new QLabel(QString("ISO Value:"));
	m_mcISOValueSlider = new QSlider(Qt::Horizontal, this);
	m_mcISOValueSlider->setMinimum(0);
	m_mcISOValueSlider->setMaximum(255);
	m_mcISOValueSlider->setEnabled(false);
	controlWidgetLayout->addWidget(m_mcISOValueLabel, 1, 0);
	controlWidgetLayout->addWidget(m_mcISOValueSlider, 1, 1, 1, 2);
	//Connect signals and slots
	connect(m_mcOpenFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
	connect(m_mcISOValueSlider, SIGNAL(valueChanged(int)), this, SLOT(onISOValueChanged(int)));
}

MarchingCubesDemo::~MarchingCubesDemo()
{

}

void MarchingCubesDemo::calculateGradient()
{
	int size = m_dataDepth * m_dataHeight*m_dataWidth;
	m_gradient.resize(size);
	for (int z = 0; z < m_dataDepth; z++) {
		for (int y = 0; y < m_dataHeight; y++) {
			for (int x = 0; x < m_dataWidth; x++) {
				int step;
				int x2 = x + 1, x1 = x - 1;
				int y2 = y + 1, y1 = y - 1;
				int z2 = z + 1, z1 = z - 1;
				int fx1,fx2, fy1,fy2, fz1,fz2;
				float stepx = 2, stepy = 2, stepz = 2;
				if (x2 >= m_dataWidth) {
					fx2 = 0;
					stepx = 1;
				}
				else {
					fx2 = m_rawData[toIndex(x2, y, z)];
				}
				if (x1 < 0) {
					fx1 = 0;
					stepx = 1;
				}
				else {
					fx1 = m_rawData[toIndex(x1, y, z)];
				}
				if (y2 >= m_dataHeight) {
					fy2 = 0;
					stepy = 1;
				}
				else {
					fy2 = m_rawData[toIndex(x, y2, z)];
				}
				if (y1 < 0) {
					fy1 = 0;
					stepy = 1;
				}
				else {
					fy1 = m_rawData[toIndex(x, y1, z)];
				}
				if (z2 >= m_dataDepth) {
					fz2 = 0;
					stepz = 1;
				}
				else {
					fz2 = m_rawData[toIndex(x, y, z2)];
				}
				if (z1 < 0) {
					fz1 = 0;
					stepz = 1;
				}
				else {
					fz1 = m_rawData[toIndex(x, y, z1)];
				}
				m_gradient[toIndex(x, y, z)] = QVector3D(-(fx2 - fx1) / stepx, -(fy2 - fy1) / stepy, -(fz2 - fz1) / stepz).normalized();
			}
		}
	}
}

int MarchingCubesDemo::toIndexAt(int x, int y, int z) {
	if (x >= 0 && x < m_dataWidth && y >= 0 && y < m_dataHeight&&z >= 0 && z < m_dataDepth)
		return toIndex(x, y, z);
	else
		return -1;
}

inline unsigned char MarchingCubesDemo::getValue(int x, int y, int z)
{
	return 0;
}

int MarchingCubesDemo::toIndex(int x, int y, int z)
{
	return x + y * m_dataWidth + z * m_dataWidth*m_dataHeight;
}


std::tuple<int, int, int> MarchingCubesDemo::toPoint(int index)
{
	int x = index % m_dataWidth;
	int z = index / (m_dataWidth*m_dataHeight);
	int y = (index - z * m_dataWidth*m_dataHeight) / m_dataWidth;
	return std::tuple<int, int, int>(x, y, z);
}

bool MarchingCubesDemo::greaterThanISO(int index, unsigned char iso)
{
	return m_rawData[index] > iso;
}

QPair<QVector<QVector3D>, QVector<QVector3D>> MarchingCubesDemo::MarcingCubes(int value)
{
	QVector<QVector3D> triangles;
	QVector<QVector3D> normals;
	for (int z = 0; z < m_dataDepth - 1; z++) {
		for (int y = 0; y < m_dataHeight - 1; y++) {
			for (int x = 0; x < m_dataWidth - 1; x++) {
				int id = 0;
				id += greaterThanISO(toIndex(x, y+1, z+1), value) ? (1 << 0) : (0);
				id += greaterThanISO(toIndex(x, y+1, z), value) ? (1 << 1) : (0);
				id += greaterThanISO(toIndex(x, y, z), value) ? (1 << 2) : (0);
				id += greaterThanISO(toIndex(x, y, z+1), value) ? (1 << 3) : (0);
				id += greaterThanISO(toIndex(x +1, y+1, z+1), value) ? (1 << 4) : (0);
				id += greaterThanISO(toIndex(x + 1, y+1, z), value) ? (1 << 5) : (0);
				id += greaterThanISO(toIndex(x +1, y , z), value) ? (1 << 6) : (0);
				id += greaterThanISO(toIndex(x + 1, y, z + 1), value) ? (1 << 7) : (0);
				for (int i = 0; m_triangleTable[id][i] != -1; i += 3) {
					//for a single triangle mesh
					for (int j = 0; j < 3; j++) {
						int e = m_triangleTable[id][i + j];
						int v1x = x + m_edgeToVertex[e][0];
						int v1y = y + m_edgeToVertex[e][1];
						int v1z = z + m_edgeToVertex[e][2];
						int v2x = x + m_edgeToVertex[e][3];
						int v2y = y + m_edgeToVertex[e][4];
						int v2z = z + m_edgeToVertex[e][5];

						int v1value = m_rawData[toIndex(v1x, v1y, v1z)];
						int v2value = m_rawData[toIndex(v2x, v2y, v2z)];
						triangles.push_back(interpulation(v1x, v1y, v1z, v2x, v2y, v2z, v1value, v2value, value));
						normals.push_back(QVector3D((m_gradient[toIndex(v1x,v1y,v1z)]+m_gradient[v2x,v2y,v2z])/2));
						//normals.push_back(m_gradient[toIndex(v1x,v1y,v1z)] +static_cast<float>(value-v1value)*(m_gradient[toIndex(v2x,v2y,v2z)]-m_gradient[toIndex(v1x,v1y,v1z)])/(v2value-v1value));
					}
				}
			}
		}
	}
	return QPair<QVector<QVector3D>, QVector<QVector3D>>(triangles, normals);
}
QVector3D MarchingCubesDemo::interpulation(int x1, int y1, int z1, int x2, int y2, int z2, int value1, int value2, int iso)
{
	return QVector3D(m_dataXSpace*(static_cast<float>(iso - value1)*(x2 - x1) / static_cast<float>(value1 - value2) + x1),
		m_dataYSpace*(static_cast<float>(iso - value1)*(y2 - y1) / float(value1 - value2) + y1),
		m_dataZSpace*(static_cast<float>(iso - value1)*(z2 - z1) / float(value1 - value2) + z1));
}

void MarchingCubesDemo::onOpenFile()
{
	QString path = QFileDialog::getOpenFileName(this, QString(".vifo file"), QString("~/Downloads/"),
		QString(".vifo (*.vifo)"));

	//No File Name
	if (path.isEmpty() == true)
		return;


	//File doesn't exist
	QFileInfo fileInfo(path);
	if (fileInfo.exists() == false)
		QMessageBox::critical(this, QString("Error"), QString("File does not exist"), QMessageBox::Ok, QMessageBox::Ok);

	QFile file(path);
	//can not be opened
	if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
		QMessageBox::critical(this, QString("Error"), QString("File can not be opened"), QMessageBox::Ok, QMessageBox::Ok);

	QString targetFile;
	QTextStream ts(&file);
	//target file info
	ts >> m_dataWidth >> m_dataHeight >> m_dataDepth;
	ts >> m_dataXSpace >> m_dataYSpace >> m_dataZSpace;
	ts >> targetFile;
	qDebug() << "TargetFileName:" << targetFile;
	qDebug() << m_dataWidth << " " << m_dataHeight << " " << m_dataDepth;
	qDebug() << m_dataXSpace << " " << m_dataYSpace << " " << m_dataZSpace;
	file.close();


	QString fileName = fileInfo.path() + "/" + targetFile;
	qDebug() << "TargetPath:" << fileName;

	QFile dataFile(fileName);
	if (dataFile.open(QIODevice::ReadOnly) == false) {
		QMessageBox::critical(this, QString("Error"), QString("File can not be opened"), QMessageBox::Ok, QMessageBox::Ok);
	}
	qint64 length = m_dataWidth * m_dataHeight*m_dataDepth;
	unsigned char * tmp = new unsigned char[length];
	qDebug() << "FileLength:" << length;
	qint64 readLength = dataFile.read((char*)(tmp), length);
	if (readLength != length) {
		QMessageBox::warning(this, QString("Warning"),
			tr("Only read %1 byte(s) of %2 byte(s)").arg(readLength).arg(length),
			QMessageBox::Ok, QMessageBox::Ok);
	}

	//set the raw pointer to the smart pointer
	m_rawData.reset(tmp);
	m_mcISOValueSlider->setEnabled(true);

	//Calculating gradient for normal vectors
	calculateGradient();

}
void MarchingCubesDemo::onISOValueChanged(int value)
{
	OpenGLWidget * widget = dynamic_cast<OpenGLWidget *>(displayWidget());
	if (widget != nullptr) {
		QPair<QVector<QVector3D>, QVector<QVector3D>> res = MarcingCubes(value);
		 widget->updateModel(res.first,res.second);
	}

}

void MarchingCubesDemo::onTestChanged()
{

}
