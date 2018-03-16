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
#include <QTextEdit>
#include <chrono>
//#define MEMORY_ARENA

QDebug operator<<(QDebug d,const Point3Df &f)
{
	QDebugStateSaver saver(d);
	d.nospace() << "(" << f.x() << "," << f.y() << "," << f.z() << ")";
	return d;
}


LoopSubdivisionSurfaceDemo::LoopSubdivisionSurfaceDemo(QWidget * parent /*= nullptr*/) :BaseDemoWidget(parent)
{
	//create control widget 
	QWidget * controlWidget = new QWidget(this);
	QGridLayout * controlLayout = new QGridLayout;

	//button
	m_openFileButton = new QPushButton;
	m_openFileButton->setText(QString("Open..."));
	//lineedit
	m_fileNamesLineEdit = new QLineEdit;
	//label
	m_label = new QLabel;
	m_label->setText(tr("File:"));
	m_fileNamesLineEdit->setReadOnly(true);

	controlLayout->addWidget(m_label, 0, 0,1,1,Qt::AlignRight);
	controlLayout->addWidget(m_fileNamesLineEdit, 0, 1);
	controlLayout->addWidget(m_openFileButton, 0, 2);

	//slider label
	m_sliderLabel = new QLabel(tr("Level:"));
	//slider
	m_slider = new QSlider(Qt::Horizontal);
	m_slider->setMaximum(10);
	m_slider->setEnabled(false);
	controlLayout->addWidget(m_sliderLabel, 1, 0,1,1,Qt::AlignRight);
	controlLayout->addWidget(m_slider, 1, 1, 1, 2);

	//text edit
	m_textEdit = new QTextEdit;
	m_textEdit->setReadOnly(true);
	controlLayout->addWidget(m_textEdit, 2, 0, 1, 3);


	controlWidget->setLayout(controlLayout);
	setControlWidget(controlWidget);

	//create display widget
	m_displayWidget = new OpenGLWidget(Camera(),this);

    m_displayWidget->setAnimation(true);
	setDisplayWidget(m_displayWidget);

	//signals and slots
	connect(m_openFileButton, SIGNAL(clicked()), this, SLOT(onOpenFile()));
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onRecursionsCountChanged(int)));

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
			//qDebug() << f + j;
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


#ifdef MEMORY_ARENA
	MemoryArena arena;
#endif


	//The following two unique_ptr vectors is to
	// hold the newly created vertices and vertex

	std::vector<std::unique_ptr<SDFace>> facesContainers;
	std::vector<std::unique_ptr<SDVertex>> verticesContainers;

	for(int level=0;level<LEVEL;level++)
	{
		//qDebug() << "Level:" << level;
		std::vector<SDFace *> newFaces;
		std::vector<SDVertex *> newVertices;

		std::vector<std::unique_ptr<SDFace>> newFacesContainers;
		std::vector<std::unique_ptr<SDVertex>> newVerticesContainers;

		//allocae next level of children in mesh tree
		for(auto vert:SDVertices)
		{

#ifndef MEMORY_ARENA
			std::unique_ptr<SDVertex> vptr(new SDVertex);
			vert->setChild(vptr.get());
			newVerticesContainers.push_back(std::move(vptr));
#else
			vert->setChild(arena.Alloc<SDVertex>());
#endif// !MEMORY_ARENA

			vert->child()->regular(vert->isRegular());//
			vert->child()->boundary(vert->isBoundary());
			newVertices.push_back(vert->child());
		}
		for(auto face:SDFaces)
		{
			std::vector<SDFace *> childrens;
			for(int i=0;i<4;i++)
			{
#ifndef MEMORY_ARENA
				std::unique_ptr<SDFace> fptr(new SDFace);
				face->children[i] = fptr.get();
				newFacesContainers.push_back(std::move(fptr));
#else
				face->children[i] = arena.Alloc<SDFace>();
#endif
				newFaces.push_back(face->children[i]);
			}
		}

		//Updating vertex position for even vertices
		//qDebug() << "update position for event vertices";
		for(auto vert:SDVertices)
		{
			if(vert->isBoundary() == true)
			{
				//Only two points are involved
				Point3Df p = weightBoundary(vert, 1.f / 8.f);
				vert->child()->setPoint(p);
				//qDebug() <<"Boundary:"<< p;
			}
			else {
				if(vert->isRegular() == true)
				{
					//A desired and common caseweightBoundary(vert, 1.f/8.f)
					Point3Df p = weightOneRing(vert, 1.f / 16.f);
					vert->child()->setPoint(weightOneRing(vert, 1.f / 16.f));
					//qDebug() << p;
					
				}else
				{
					Point3Df p = weightOneRing(vert, beta(vert->valence()));
					vert->child()->setPoint(weightOneRing(vert, beta(vert->valence())));
					//qDebug() << p<< beta(vert->valence());
				}
			}
		}
		//Compute the positions of the odd vertices
		//qDebug() << "Compute the Position of the odd vertices:";
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
#ifndef MEMORY_ARENA
					std::unique_ptr<SDVertex> vptr(new SDVertex);
					newVertOnEdge = vptr.get();
					newVerticesContainers.push_back(std::move(vptr));
#else
					newVertOnEdge = arena.Alloc<SDVertex>();
#endif
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
						
						Point3Df p = 0.5f*e.v0()->point() +
							0.5f*e.v1()->point();
						newVertOnEdge->setPoint(p);
						//qDebug() <<"Boundary:"<< p;
	
					}else
					{
						Point3Df p = (3.f / 8.f)*e.v0()->point() +
							(3.f / 8.f)*e.v1()->point() +
							(1.f/8.f)*face->otherVertex(e.v0(), e.v1())->point() +
							(1.f/8.f)*face->f[i]->otherVertex(e.v0(), e.v1())->point();
						newVertOnEdge->setPoint(p);
						//qDebug() <<"Interior"<< p;
						
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
#ifndef MEMORY_ARENA
		facesContainers = std::move(newFacesContainers);
		verticesContainers = std::move(newVerticesContainers);
#endif


	}

	//Transform vertices to triangles and create normal vectors
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
	auto indices = m_model->getFaceIndices();
	auto vertices = m_model->getVertices();
	std::vector<int> idxs;
	for (auto & v : indices)
	{
		for (auto i : v)
		{
			idxs.push_back(i);
		}
	}

	std::vector<Point3Df> vert;
	for (auto & v : vertices)
	{
            vert.push_back({ v.x(),v.y(),v.z() });
	}
	auto star = std::chrono::system_clock::now();

	auto res = LoopSubdivision(idxs, vert, value);

	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - star);
	OpenGLWidget * widget = static_cast<OpenGLWidget*>(displayWidget());
	if (widget != nullptr)
	{
		widget->updateModel(res.first, res.second);

		QString resInfo;
        resInfo += QString::number(res.first.size()) + " face(s) in total.\n";
		int sec = (duration.count())*std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        resInfo += "Time Cost: " + QString::number(sec) + " s.";
		m_textEdit->setText(resInfo);
	}
}

//QVector<QVector3D> toLine(const QVector<QVector3D> & triangles)
//{
//	QVector<QVector3D> res;
//	assert(triangles.size()%3 == 0);
//	for(int i=0;i<triangles.size();i+=3)
//	{
//		res.push_back(triangles[i]);
//		res.push_back(triangles[i+1]);
//		res.push_back(triangles[i+1]);
//		res.push_back(triangles[i+2]);
//		res.push_back(triangles[i+2]);
//		res.push_back(triangles[i]);
//	}
//	return res;
//}
void LoopSubdivisionSurfaceDemo::onOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("Obj File"), QString("."), QString(".obj(*.obj)"));
	if (fileName.isEmpty() == true)
		return;
	m_fileNamesLineEdit->setText(fileName);
	m_model.reset(new Model(fileName.toStdString()));
	if (m_model->isLoaded() == false)
	{
		QMessageBox::critical(this, QString("Error"), QString(".obj file can not be loaded"), QMessageBox::Ok, QMessageBox::Ok);
	}
	m_slider->setEnabled(true);
	onRecursionsCountChanged(0);
}
