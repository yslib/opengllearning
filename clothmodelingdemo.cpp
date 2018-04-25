#include "clothmodelingdemo.h"
#include "basedemowidget.h"
#include "openglwindow.h"
#include "utils.h"
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>



void ClothModelingDemo::initModel()
{

	m_meshSize = 20;
	m_springLength = 1.5;
	m_gravity = Vector3f(0,-0.98,0);
	m_pointMass = 0.01f;
	m_damping = 0.9f;
	m_springConst = 20.0;
	m_euler = true;

	m_nVerts = m_meshSize * m_meshSize;
	// there are springs going right for each vertex
	// not on right edge, springs going down for each vertex
	// not on bottom edge
	m_nSprings = (m_meshSize - 1) * m_meshSize * 2;

	// cross springs
	m_nSprings += (m_meshSize - 1) * (m_meshSize - 1) * 2;

	// skipping springs
	m_nSprings += (m_meshSize - 2) * m_meshSize * 2;

	m_oldVertices.resize(m_nVerts);
	m_updatedVertices.resize(m_nVerts);
	m_springs.resize(m_nSprings);

	m_nIndices = (m_meshSize - 1)*(m_meshSize - 1) * 2 * 3;
	resetCloth();
	std::string path = "D:\\teapot.obj";
	ObjReader model(path);
	if (model.isLoaded() == false)
	{
		QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Can not open obj file"));
		QCoreApplication::exit(-1);
	}
	Trans3DMat mat;
	mat.rotate(-90, Vector3f(1, 0, 0));
	TriangleMesh mesh(model.getVerticesFlatArray(),
		model.getNormalsFlatArray(),
		model.getVertexCount(),
		model.getFacesIndicesFlatArray(),
		model.getFacesCount(), mat);

	m_nModelVerts = mesh.getVertexCount();
	m_nModelIndices = mesh.getIndexCount();
	//m_nModelVerts = 0;
	//m_nModelIndices = 0;
	//for cloth vertices and model vertices
	m_vertice.resize(m_nVerts + m_nModelVerts);
	m_normals.resize(m_nVerts + m_nModelVerts);
	m_indices.resize(m_nIndices + m_nModelIndices);

	//set model vertex and corresponding normals
	for (int i = 0; i <m_nModelVerts ; i++) {
		m_vertice[i] = mesh.getVerticesArray()[i];
		m_normals[i] = mesh.getNormalsArray()[i];
	}
	//set model vertex index
	for (int i = 0; i < m_nModelIndices; i++) {
		m_indices[i] = mesh.getIndicesArray()[i];
	}
	//set cloth vertex index
	auto id = m_nModelIndices;
	for (auto i = 0; i < m_meshSize - 1; i++) {
		for (auto j = 0; j < m_meshSize - 1; j++) {
			const auto id1 = i * m_meshSize + j;
			const auto id2 = i * m_meshSize + j + 1;
			const auto id3 = (i + 1)*m_meshSize + j;
			const auto id4 = (i + 1)*m_meshSize + j + 1;
			m_indices[id++] = id1+m_nModelVerts;
			m_indices[id++] = id2+m_nModelVerts;
			m_indices[id++] = id3+m_nModelVerts;
			m_indices[id++] = id2+m_nModelVerts;
			m_indices[id++] = id4+m_nModelVerts;
			m_indices[id++] = id3+m_nModelVerts;
		}
	}

	

	std::vector<std::shared_ptr<Shape>> lightShapes;
	MaterialReader mrd;
	std::vector<std::shared_ptr<Shape>> triangles = Triangle::createTriangleMesh(model.getVerticesFlatArray(),
		model.getNormalsFlatArray(),
		model.getVertexCount(),
		model.getFacesIndicesFlatArray(),
		model.getFacesCount(),
		model.getIndexToMtlName(),
		mrd, &lightShapes, mat);
	m_aggregate = std::make_shared<BVHTreeAccelerator>(triangles);
	m_scene = std::make_shared<Scene>(m_aggregate, std::vector<std::shared_ptr<AreaLight>>());

}

void ClothModelingDemo::resetCloth()
{



	for (int i = 0; i < m_meshSize; i++) {
		for (int j = 0; j < m_meshSize; j++) {
			SpringVertex & v = m_oldVertices[i * m_meshSize + j];
			Point3f p = { float(j) - float(m_meshSize - 1) / 2, 20, float(i) - float(m_meshSize - 1) / 2 };
			v.pos = p;
			v.vel = { 0,0,0 };
			v.mass = m_pointMass;
			v.fixed = false;
			v.normal = { 0,0,0 };
		}
	}

	// fix four corners
	m_oldVertices[0].fixed = true;
	m_oldVertices[m_meshSize - 1].fixed = true;
	//m_oldVertices[m_meshSize * (m_meshSize - 1)].fixed = true;
	//m_oldVertices[m_meshSize * m_meshSize - 1].fixed = true;

	for (int i = 0; i < m_nVerts; i++) {
		m_updatedVertices[i] = m_oldVertices[i];
	}

	// othogonal springs
	int id = 0;
	for (int i = 0; i < m_meshSize; i++) {
		for (int j = 0; j < m_meshSize - 1; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = i * m_meshSize + j + 1;

			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength;
			++id;
		}
	}

	for (int i = 0; i < m_meshSize - 1; i++) {
		for (int j = 0; j < m_meshSize; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = (i + 1) * m_meshSize + j;
			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength;
			id++;
		}
	}

	// cross springs
	for (int i = 0; i < m_meshSize - 1; i++) {
		for (int j = 0; j < m_meshSize - 1; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = (i + 1) * m_meshSize + j + 1;
			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength * sqrt(2.0f);
			id++;
		}
	}

	for (int i = 0; i < m_meshSize - 1; i++) {
		for (int j = 1; j < m_meshSize; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = (i + 1) * m_meshSize + j - 1;
			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength * sqrt(2.0f);
			id++;
		}
	}

	// skip springs
	for (int i = 0; i < m_meshSize; i++) {
		for (int j = 0; j < m_meshSize - 2; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = i * m_meshSize + j + 2;
			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength * 2;
			id++;
		}
	}

	for (int i = 0; i < m_meshSize - 2; i++) {
		for (int j = 0; j < m_meshSize; j++) {
			m_springs[id].p1 = i * m_meshSize + j;
			m_springs[id].p2 = (i + 2) * m_meshSize + j;

			m_springs[id].k = m_springConst;
			m_springs[id].x = m_springLength * 2;
			id++;
		}
	}
}

void ClothModelingDemo::simulate()
{
	int msTimeSinceLastFrame = 11;
	float timeSinceLastFrame = msTimeSinceLastFrame / 1000.0;

	double timeSinceLastUpdate = 0.0;
	timeSinceLastUpdate += msTimeSinceLastFrame;

	bool updated = false;

	while (timeSinceLastUpdate > 10.0) {
		timeSinceLastUpdate -= 10.0;
		float timePassedInSeconds = 0.005f;
		updated = true;

		//evaluate tension for all springs
		for (int i = 0; i < m_nSprings; i++) {
			float length = (m_oldVertices[m_springs[i].p1].pos - m_oldVertices[m_springs[i].p2].pos).length();
			float dis = length - m_springs[i].x;
			m_springs[i].tension = m_springs[i].k * (dis / m_springs[i].x);
		}

		//
		for (int i = 0; i < m_nVerts; i++) {
			m_updatedVertices[i].fixed = m_oldVertices[i].fixed;
			m_updatedVertices[i].mass = m_oldVertices[i].mass;

			//set the velocity of fixed point as 0 
			if (m_oldVertices[i].fixed) {

				m_updatedVertices[i].pos = m_oldVertices[i].pos;
				m_updatedVertices[i].vel = { 0,0,0 };
			}
			else {
				Point3f pi = m_oldVertices[i].pos;
				Vector3f vi = m_oldVertices[i].vel;

				Point3f x = pi;
				Vector3f v = vi;
				

				Vector3f forces = m_gravity;
				for (int j = 0; j < m_nSprings; j++) {
					Vector3f dir = { 0, 0, 0 };
					if (m_springs[j].p1 == i) {
						dir = (m_oldVertices[m_springs[j].p2].pos - pi).normalized();
					}
					else if (m_springs[j].p2 == i) {
						dir = (m_oldVertices[m_springs[j].p1].pos - pi).normalized();
					}
					forces = forces + m_springs[j].tension * dir;
				}

				//gravity + tension

				Vector3f acc = forces / m_oldVertices[i].mass;


				rungeKutta4(&x,&v, acc, timePassedInSeconds);


				Ray aRay(v,x);
				Float t;
				Interaction isect;
				if(m_scene->intersect(aRay,&t,&isect) == true)
				{
					if(t <0.1)
					{
						m_updatedVertices[i].fixed = true;
					}
				}

				m_updatedVertices[i].pos = x;
				m_updatedVertices[i].vel = v;
			}
		}

		for (int i = 0; i < m_nVerts; i++) {
			m_oldVertices[i] = m_updatedVertices[i];
		}
	}

	if (updated) {
		for (int i = 0; i < m_nVerts; i++) 
			m_oldVertices[i].normal = { 0,0,0 };
		//int id = 0;
		for (int i = 0; i < m_meshSize - 1; i++) {
			for (int j = 0; j < m_meshSize - 1; j++) {
				int id1 = i * m_meshSize + j;
				int id2 = i * m_meshSize + j + 1;
				int id3 = (i + 1)*m_meshSize + j;
				int id4 = (i + 1)*m_meshSize + j + 1;
				const Point3f& p0 = m_oldVertices[id1].pos;
				const Point3f& p1 = m_oldVertices[id2].pos;
				const Point3f& p2 = m_oldVertices[id3].pos;
				const Point3f& p3 = m_oldVertices[id4].pos;

				Vector3f tempNormal = Vector3f::crossProduct((p2 - p0), (p1 - p0));
				Vector3f tempNormal2 = Vector3f::crossProduct((p3 - p2), (p2 - p2));

				m_oldVertices[id1].normal = tempNormal.normalized();
				m_oldVertices[id2].normal =(tempNormal + tempNormal2).normalized();
				m_oldVertices[id3].normal = (tempNormal + tempNormal2).normalized();
				m_oldVertices[id4].normal = tempNormal2.normalized();

			}
		}
		//
		for (int i = 0; i < m_nVerts; i++) {
			m_vertice[i+m_nModelVerts] = m_oldVertices[i].pos;
			m_normals[i+m_nModelVerts] = m_oldVertices[i].normal;
		}
	}

	m_displayWidget->setTriangleMesh(m_vertice.data(), m_normals.data(), m_nVerts+m_nModelVerts, (unsigned int*)m_indices.data(), m_indices.size());

}
ClothModelingDemo::ClothModelingDemo(QWidget* parent):
	BaseDemoWidget(parent),
	m_timer(nullptr),
	m_once(false),
	m_euler(false)
{
	//display widget intialization
	m_displayWidget = new OpenGLWidget(Camera(Vector3f(30,30,30),Vector3f(0,1,0),-130.4,-24.5),this);
	setDisplayWidget(m_displayWidget);
	m_displayWidget->setAnimation(true);

	//control widget initialization
	QWidget * m_controlWidget = new QWidget(this);
	QVBoxLayout * m_layout = new QVBoxLayout;
	m_elasticity = new TitledSliderWithSpinBox(m_controlWidget, QStringLiteral("Elasticity:"));
	m_integrationMethod = new QComboBox(m_controlWidget);
	m_integrationMethod->addItem(QStringLiteral("Euler Integration"));
	m_integrationMethod->addItem(QStringLiteral("Runge Kutta method"));
	m_reset = new QPushButton(QStringLiteral("Reset"));
	m_layout->addWidget(m_elasticity);
	m_layout->addWidget(m_integrationMethod);
	m_layout->addWidget(m_reset);
	m_controlWidget->setLayout(m_layout);
	setControlWidget(m_controlWidget);
	m_timer = new QTimer(this);
	connect(m_elasticity,SIGNAL(valueChanged(int)),this,SLOT(onSliderChanged(int)));
	connect(m_reset, SIGNAL(clicked()), this, SLOT(onResetButton()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(simulation()));

	initModel();
	
}

void ClothModelingDemo::onSliderChanged(int value)
{
	//qDebug() << value;
	m_springConst = value;
}

void ClothModelingDemo::simulation()
{
	simulate();
}
void ClothModelingDemo::onResetButton()
{
	if(m_timer->isActive() == false){
		resetCloth();
		m_timer->setInterval(10);
		m_timer->start();
	}
}

