#include "loopsubdivisionsurfacedemo.h"
#include <set>
#include <map>
#include <memory>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include "model.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
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

	m_sliderLabel = new QLabel(tr("Recursions Count:"));
	m_slider = new QSlider(Qt::Horizontal);
	m_slider->setMaximum(10);

	controlLayout->addWidget(m_sliderLabel, 1, 0);
	controlLayout->addWidget(m_slider, 1, 1,1,2);

	controlWidget->setLayout(controlLayout);
	setControlWidget(controlWidget);


	//create display widget
	m_displayWidget = new OpenGLWidget(this);
	setDisplayWidget(m_displayWidget);

	//signals and slots
	connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
	connect(m_slider, SIGNAL(valueChanged(int value)), this, SLOT(onRecursionsCountChanged(int value)));

}

QVector<QVector3D> LoopSubdivisionSurfaceDemo::LoopSubdivision(const std::vector<int>& vertexIndices, const std::vector<Point3Df>& vertices)
{
	std::vector<SDVertex *> SDVertices; // Containing pointers to vertex
	std::vector<SDFace *> SDFaces;		//Containing pointers to face
	std::unique_ptr<SDVertex[]> SDVerticesContainer(new SDVertex[vertices.size()]);

	//Constructing vertex
	for (int i = 0; i < vertices.size(); i++) {
		SDVerticesContainer[i] = SDVertex(vertices[i]);
		SDVertices.push_back(&SDVerticesContainer[i]);
	}

	int faceCount = vertexIndices.size() / 3;

	//allocating for faces
	std::unique_ptr<SDFace[]> SDFacesContainer(new SDFace[faceCount]);

	//Constructing face
	//[1]:join the vertex belonging to the same face
	for (int i = 0, f = 0; i < faceCount; i++,f+=3) {
		std::vector<SDVertex *> vert;
		for (int j = 0; j < 3; j++) {
			SDVertex * v = &SDVerticesContainer[vertexIndices[f + j]];
			vert.push_back(v);
			//There must be setting a initial face for a vertex,any is ok.
			v->setStartFace(&SDFacesContainer[i]);
		}
		SDFacesContainer[i] = SDFace(vert);
	}

	for (int i = 0; i < faceCount; i++) {
		SDFaces.push_back(&SDFacesContainer[i]);
	}

	//[2]. join the adjacent face for a face
	std::set<SDEdge> hash;
	for(int i=0;i<faceCount;i++)
	{
		SDFace * face = SDFaces[i];
		for(int j=0;j<3;j++)
		{
			SDEdge theEdge(face->vertices()[j], face->vertices()[NEXT(j)]);
			if(hash.find(theEdge) == hash.end())
			{
				//this is a new adge
				theEdge.setEdgeNum(j);
				theEdge.setF0(face);
				hash.insert(theEdge);
			}
			else
			{
				int theNum = theEdge.edgeNum();
				theEdge.f0()->adjacentFaces()[theNum] = face;
				face->adjacentFaces()[j] = theEdge.f0();
				hash.erase(theEdge);
			}

		}
	}

	//vertex regular and boundary
	for(int i=0;i<vertices.size();i++)
	{
		SDVertex * vert = SDVertices[i];
		SDFace * face = vert->startFace();
		//find the vertex if it is a boundary vertex
		//while (face != nullptr && (face = face->nextFace(vert)) != vert->startFace());
		do
		{
			face = face->nextFace(vert);
		} while (face != nullptr && face != vert->startFace());
		vert->boundary(face == nullptr);

		if(vert->isBoundary() == true && vert->valence() == 4)
		{
			vert->regular(true);
		}else if(vert->isBoundary() == false && vert->valence() == 6)
		{
			vert->regular(true);
		}else
		{
			vert->regular(false);
		}
	}
	MemoryArena arena;
	constexpr int LEVEL = 10;
	for(int level=0;level<LEVEL;level++)
	{
		std::vector<SDFace *> newFaces;
		std::vector<SDVertex *> newVertices;


		//allocae next level of children in mesh tree
		for(auto vert:SDVertices)
		{
			vert->setChild(arena.Alloc<SDVertex>());
			vert->Child()->regular(true);
			vert->Child()->boundary(vert->isBoundary());
			newVertices.push_back(vert->Child());
		}
		for(auto face:SDFaces)
		{
			std::vector<SDFace *> childrens;
			for(int i=0;i<4;i++)
			{
				childrens.push_back(arena.Alloc<SDFace>());
			}
			face->setChildrenFaces(childrens);
		}


		//Updating vertex position for even vertices
		for(auto vert:SDVertices)
		{
			if(vert->isBoundary() == true)
			{
				//Only two points are involved
				vert->Child()->setPoint(weightBoundary(vert, 1.f/8.f));
			}
			else {
				if(vert->isRegular() == true)
				{
					//A desired and common case
					vert->Child()->setPoint(weightOneRing(vert, 1.f / 16.f));
					
				}else
				{
					vert->Child()->setPoint(weightOneRing(vert, beta(vert->valence())));
				}
			}
		}

		//Compute the positionos of the odd vertices

		std::map<SDEdge, SDVertex *> edgeVerts;// Value SDVertex * is the generated vertex on edge
		for(auto face:SDFaces)
		{
			for(int i=0;i<3;i++) 
			{
				SDEdge e(face->vertices()[i], face->vertices()[NEXT(i)]);
				SDVertex * newVertOnEdge = edgeVerts[e];
				if(newVertOnEdge != nullptr)
				{

				}else
				{
					newVertOnEdge = arena.Alloc<SDVertex>();
					newVertices.push_back(newVertOnEdge);

					/*Why: the number of adjacent vertices of the 
					new vertex is either 4 or 6,both are regular.*/
					newVertOnEdge->regular(true);	
					//If there is a nullptr among adjfacent faces of current face, the vertex is boundary vertex
					newVertOnEdge->boundary(face->adjacentFaces()[i] == nullptr);
					newVertOnEdge->setStartFace(face->childrenFaces()[3]);

					///TODO:aply edge rules to compute new vertex position
					if(newVertOnEdge->isBoundary() == true)
					{
						//two vertices are involved
						newVertOnEdge->setPoint(0.5f*e.v0()->point()+
							0.5f*e.v1()->point());
	
					}else
					{
						newVertOnEdge->setPoint(
							3.f / 8.f *e.v0()->point() +
							3.f / 8.f*e.v1()->point() +
							face->otherVertex(e.v0(), e.v1())->point() +
							face->adjacentFaces()[i]->otherVertex(e.v0(), e.v1())->point()
						);
					}

					edgeVerts[e] = newVertOnEdge;
				}
			}
		}
	}

	return QVector<QVector3D>();
}

void LoopSubdivisionSurfaceDemo::onRecursionsCountChanged(int value)
{
	
}

void LoopSubdivisionSurfaceDemo::onOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("."), QString(".obj (*.obj)"));
	if (fileName.isEmpty() == true)
		return;
	m_model.reset(new Model(fileName.toStdString()));
	if(m_model->isLoaded() == false)
	{
		QMessageBox::critical(this, QString("Error"), QString(".obj file can not be loaded"), QMessageBox::Ok, QMessageBox::Ok);
	}

}

//void LoopSubdivisionSurfaceDemo::onOpenFile()
//{
//
//}
//
