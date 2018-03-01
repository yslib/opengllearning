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

	//Test Button
	QPushButton * testButton = new QPushButton(this);
	testButton->setText("Test");
	controlLayout->addWidget(testButton, 2, 0,1,3);

	//


	//create display widget
	m_displayWidget = new OpenGLWidget(this);
	m_displayWidget->setAnimation(true);
	setDisplayWidget(m_displayWidget);

	//signals and slots
	connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onRecursionsCountChanged(int)));
	//connect(testButton, SIGNAL(clicked()), this, SLOT(onTestButton()));

}

QPair<QVector<QVector3D>,QVector<QVector3D>> 
LoopSubdivisionSurfaceDemo::LoopSubdivision(
	const std::vector<int>& vertexIndices,
	const std::vector<Point3Df>& vertices,int LEVEL)
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
		SDFace * face = &SDFacesContainer[i];
		for (int j = 0; j < 3; j++) {
			SDVertex * v = &SDVerticesContainer[vertexIndices[f + j]];
			qDebug() << f + j;
			SDFacesContainer[i].v[j] = v;
			//There must be setting a initial face for a vertex,any is ok.
			v->setStartFace(&SDFacesContainer[i]);
		}
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
			SDEdge theEdge(face->v[j], face->v[NEXT(j)]);
			if(hash.find(theEdge) == hash.end())
			{
				//this is a new adge
				theEdge.setEdgeNum(j);
				theEdge.setF0(face);
				hash.insert(theEdge);
			}
			else
			{
				theEdge = *hash.find(theEdge);
				int theNum = theEdge.edgeNum();
				theEdge.f0()->f[theNum] = face;
				face->f[j] = theEdge.f0();
				hash.erase(theEdge);
			}

		}
	}

	//vertex regular and boundary
	for(int i=0;i<vertices.size();i++)
	{
		SDVertex * vert = SDVertices[i];
		SDFace * face = vert->startFace();
		qDebug() << face;
		//find the vertex if it is a boundary vertex
		//while (face != nullptr && (face = face->nextFace(vert)) != vert->startFace());
		do
		{
			face = face->nextFace(vert);
			qDebug() << face;
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


	for(int level=0;level<LEVEL;level++)
	{
		std::vector<SDFace *> newFaces;
		std::vector<SDVertex *> newVertices;
		//allocae next level of children in mesh tree
		for(auto vert:SDVertices)
		{
			vert->setChild(arena.Alloc<SDVertex>());
			vert->child()->regular(vert->isRegular());//
			vert->child()->boundary(vert->isBoundary());
			newVertices.push_back(vert->child());
		}
		for(auto face:SDFaces)
		{
			std::vector<SDFace *> childrens;
			for(int i=0;i<4;i++)
			{
				face->children[i] = arena.Alloc<SDFace>();
				newFaces.push_back(face->children[i]);
			}
		}

		//Updating vertex position for even vertices
		for(auto vert:SDVertices)
		{
			if(vert->isBoundary() == true)
			{
				//Only two points are involved
				vert->child()->setPoint(weightBoundary(vert, 1.f/8.f));
			}
			else {
				if(vert->isRegular() == true)
				{
					//A desired and common case
					vert->child()->setPoint(weightOneRing(vert, 1.f / 16.f));
					
				}else
				{
					vert->child()->setPoint(weightOneRing(vert, beta(vert->valence())));
				}
			}
		}
		//Compute the positions of the odd vertices
		std::map<SDEdge, SDVertex *> edgeVerts;// Value SDVertex * is the generated vertex on edge
		for(auto face:SDFaces)
		{
			for(int i=0;i<3;i++)		//each edge
			{
				SDEdge e(face->v[i], face->v[NEXT(i)]);
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
					newVertOnEdge->boundary(face->f[i] == nullptr);
					newVertOnEdge->setStartFace(face->children[3]);

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
							face->f[i]->otherVertex(e.v0(), e.v1())->point()
						);
					}

					edgeVerts[e] = newVertOnEdge;
				}
			}
		}

		//update topology
		//[1]. update startFace of newly created vertex
		for(auto vert:SDVertices)
		{

			///TODO:Check here carefully
			int vnum = vert->startFace()->indexOfVertex(vert);
			vert->child()->setStartFace(vert->startFace()->children[vnum]);

		}

		//[2]/ update adjacent faces of newly created face
		for(auto face:SDFaces)
		{
			for(int i=0;i<3;i++)
			{
				//interior
				face->children[3]->f[i] =
					face->children[NEXT(i)];
				face->children[i]->f[NEXT(i)] = face->children[3];
				//exterior
				SDFace * adjacentFace = face->f[i];
				face->children[i]->f[i] =
					adjacentFace ? adjacentFace->children[adjacentFace->indexOfVertex(face->v[i])]: nullptr;
				adjacentFace = face->f[PREV(i)];
				face->children[i]->f[PREV(i)] =
					adjacentFace ? adjacentFace->children[adjacentFace->indexOfVertex(face->v[i])] : nullptr;
			}
		}
		//[3] update children face vertices
		for(auto face:SDFaces)
		{
			for(int i=0;i<3;i++)
			{
				face->children[i]->v[i] = face->v[i]->child();

				SDVertex * vert = edgeVerts[SDEdge(face->v[i], face->v[NEXT(i)])];
				face->children[i]->v[NEXT(i)] = vert;
				face->children[NEXT(i)]->v[i] = vert;
				face->children[3]->v[i] = vert;
			}
		}

		SDVertices = newVertices;
		SDFaces = newFaces;

	}

	QVector<QVector3D> v,n;
	for(auto face :SDFaces)
	{
		Point3Df pp0 = face->v[0]->point(), pp1 = face->v[1]->point(), pp2 = face->v[2]->point();

		QVector3D p0(pp0.x(), pp0.y(), pp0.z());
		QVector3D p1(pp1.x(), pp1.y(), pp1.z());
		QVector3D p2(pp2.x(), pp2.y(), pp2.z());

		QVector3D v1 = p1 - p0, v2 = p2 - p1;
		QVector3D nor = QVector3D::crossProduct(v2, v1).normalized();

		v.push_back(p0);
		v.push_back(p1);
		v.push_back(p2);
		n.push_back(-nor);
		n.push_back(-nor);
		n.push_back(-nor);
	}
	return QPair<QVector<QVector3D>, QVector<QVector3D>>(v, n);
}

void LoopSubdivisionSurfaceDemo::onRecursionsCountChanged(int value)
{
	onTestButton(value);
}

void LoopSubdivisionSurfaceDemo::onTestButton(int level)
{
	std::vector<Point3Df> vertices =
	{
	{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f,  0.5f, -0.5f },
	{ 0.5f,  0.5f, -0.5f } ,
	{ -0.5f,  0.5f, -0.5f } ,
	{ -0.5f, -0.5f, -0.5f } ,

	{ -0.5f, -0.5f,  0.5f },
	{ 0.5f, -0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{ -0.5f, -0.5f,  0.5f },

	{ -0.5f,  0.5f,  0.5f },
	{ -0.5f,  0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f } ,
	{ 0.5f, -0.5f, -0.5f } ,
	{ 0.5f, -0.5f,  0.5f } ,
	{ 0.5f,  0.5f,  0.5f } ,
	{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f,  0.5f },
	{ 0.5f, -0.5f,  0.5f } ,
	{ -0.5f, -0.5f,  0.5f } ,
	{ -0.5f, -0.5f, -0.5f }  ,

	{ -0.5f,  0.5f, -0.5f }  ,
	{ 0.5f,  0.5f, -0.5f }  ,
	{ 0.5f,  0.5f,  0.5f }  ,
	{ 0.5f,  0.5f,  0.5f } ,
	{ -0.5f,  0.5f,  0.5f } ,
	{ -0.5f,  0.5f, -0.5f }  ,
	};

	QVector<QVector3D> a =
	{
		{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f,  0.5f, -0.5f },
	{ 0.5f,  0.5f, -0.5f } ,
	{ -0.5f,  0.5f, -0.5f } ,
	{ -0.5f, -0.5f, -0.5f } ,

	{ -0.5f, -0.5f,  0.5f },
	{ 0.5f, -0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{ -0.5f, -0.5f,  0.5f },

	{ -0.5f,  0.5f,  0.5f },
	{ -0.5f,  0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f,  0.5f },
	{ -0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f,  0.5f },
	{ 0.5f,  0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f } ,
	{ 0.5f, -0.5f, -0.5f } ,
	{ 0.5f, -0.5f,  0.5f } ,
	{ 0.5f,  0.5f,  0.5f } ,
	{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f,  0.5f },
	{ 0.5f, -0.5f,  0.5f } ,
	{ -0.5f, -0.5f,  0.5f } ,
	{ -0.5f, -0.5f, -0.5f }  ,

	{ -0.5f,  0.5f, -0.5f }  ,
	{ 0.5f,  0.5f, -0.5f }  ,
	{ 0.5f,  0.5f,  0.5f }  ,
	{ 0.5f,  0.5f,  0.5f } ,
	{ -0.5f,  0.5f,  0.5f } ,
	{ -0.5f,  0.5f, -0.5f }  ,
	};
	QVector<QVector3D> b = {
		{ 0.0f,  0.0f, -1.0f },
	{ 0.0f,  0.0f, -1.0f },
	{ 0.0f,  0.0f, -1.0f },
	{ 0.0f,  0.0f, -1.0f },
	{ 0.0f,  0.0f, -1.0f },
	{ 0.0f,  0.0f, -1.0f },

	{ 0.0f,  0.0f,  1.0f },
	{ 0.0f,  0.0f,  1.0f },
	{ 0.0f,  0.0f,  1.0f },
	{ 0.0f,  0.0f,  1.0f },
	{ 0.0f,  0.0f,  1.0f },
	{ 0.0f,  0.0f,  1.0f },

	{ -1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },

	{ 1.0f,  0.0f,  0.0f },
	{ 1.0f,  0.0f,  0.0f },
	{ 1.0f,  0.0f,  0.0f },
	{ 1.0f,  0.0f,  0.0f },
	{ 1.0f,  0.0f,  0.0f },
	{ 1.0f,  0.0f,  0.0f },

	{ 0.0f, -1.0f,  0.0f },
	{ 0.0f, -1.0f,  0.0f },
	{ 0.0f, -1.0f,  0.0f },
	{ 0.0f, -1.0f,  0.0f },
	{ 0.0f, -1.0f,  0.0f },
	{ 0.0f, -1.0f,  0.0f },

	{ 0.0f,  1.0f,  0.0f },
	{ 0.0f,  1.0f,  0.0f },
	{ 0.0f,  1.0f,  0.0f },
	{ 0.0f,  1.0f,  0.0f },
	{ 0.0f,  1.0f,  0.0f },
	{ 0.0f,  1.0f,  0.0f }
	};


	// tetrahedron
	std::vector<Point3Df> c1 = {
	{0.f,0.f,0.f},
	{0.5f,0.f,0.0f},
	{0.f,0.f,-0.5f},
	{0.f,0.5f,0.0f}
	};
	std::vector<int> i1 = {0,1,3,1,0,2,2,3,1,3,2,0};


	//triangle
	std::vector<Point3Df> c = {
		{-.5f,.0f,.0f},
	{0.5f,.0f,0.0f},
	{0.0f,0.866f,0.0f}
	};
	std::vector<int> i = {0,1,2};

	//two triangle

	std::vector<Point3Df> c2 = {
		{0.0f,0.0f,0.0f},
	{1.f,0.f,0.f},
	{0.f,1.0f,0.f},
	{0.0f,0.0f,-1.0f}
	};
	std::vector<int> i2 = {0,1,2,3,0,2};


	std::vector<Point3Df> c3 = {
		{0.f,0.f,0.f},
	{1.f,1.f,0.f},
	{0.f,2.f,0.f},
	{-1.f ,1.f,0.f}
	};
	std::vector<int> i3 = {0,1,2,0,2,3};

	
	std::vector<Point3Df> c4 = {
		{ 0.f,0.f,0.f },
	{ 1.f,1.f,0.f },
	{ 0.f,2.f,0.f }
	
	};
	std::vector<int> i4 = { 0,1,2 };

	auto res = LoopSubdivision(i4,c4,level);
	qDebug() << res.first.size();
	OpenGLWidget * widget = dynamic_cast<OpenGLWidget*>(displayWidget());
	if(widget != nullptr)
	{
		widget->updateModel(res.first,res.second);
	}
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
