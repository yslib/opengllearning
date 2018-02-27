#include "loopsubdivisionsurfacedemo.h"
#include <unordered_set>
#include <memory>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

LoopSubdivisionSurfaceDemo::LoopSubdivisionSurfaceDemo(QWidget * parent /*= nullptr*/):BaseDemoWidget(parent)
{
	//create control widget 
	QWidget * controlWidget = new QWidget(this);
	QGridLayout * controlLayout = new QGridLayout;
	m_openFileButton = new QPushButton;
	m_fileNamesLineEdit = new QLineEdit;
	m_label = new QLabel;
	m_label->setText(tr("File:"));
	m_fileNamesLineEdit->setReadOnly(true);

	controlLayout->addWidget(m_label, 0, 0);
	controlLayout->addWidget(m_fileNamesLineEdit, 0, 1);
	controlLayout->addWidget(m_openFileButton, 0, 2);
	controlWidget->setLayout(controlLayout);
	setControlWidget(controlWidget);


	//create display widget
	m_displayWidget = new OpenGLWidget(this);
	setDisplayWidget(m_displayWidget);

	//signals and slots
	connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
}

void LoopSubdivisionSurfaceDemo::LoopSubdivision(const std::vector<int>& vertexIndices, const std::vector<Point3Df>& vertices)
{
	std::vector<SDVertex *> SDVertices;
	std::vector<SDFace *> SDFaces;
	//allocating for vertices 
	std::unique_ptr<SDVertex[]> SDVerticesContainer(new SDVertex[vertices.size()]);
	for (int i = 0; i < vertices.size(); i++) {
		SDVerticesContainer[i] = SDVertex(vertices[i]);
		SDVertices.push_back(&SDVerticesContainer[i]);
	}

	int faceCount = vertexIndices.size() / 3;

	//allocating for faces
	std::unique_ptr<SDFace[]> SDFacesContainer(new SDFace[faceCount]);

	//join the vertex to face
	for (int i = 0, f = 0; i < faceCount; i++,f+=3) {
		std::vector<SDVertex *> vert;
		for (int j = 0; j < 3; j++) {
			SDVertex * v = &SDVerticesContainer[vertexIndices[f + j]];
			vert.push_back(v);
			v->setStartFace(&SDFacesContainer[i]);
		}
		SDFacesContainer[i] = SDFace(vert);
	}

	for (int i = 0; i < faceCount; i++) {
		SDFaces.push_back(&SDFacesContainer[i]);
	}
}

//void LoopSubdivisionSurfaceDemo::onOpenFile()
//{
//
//}
//
