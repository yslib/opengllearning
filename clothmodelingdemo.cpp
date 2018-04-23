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

void ClothModelingDemo::initTestModel()
{
	std::string path="D:\\teapot-low.obj";
	ObjReader model(path);
	if(model.isLoaded() == false)
	{
		QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Can not open obj file"));
		QCoreApplication::exit(-1);
	}
	Trans3DMat mat;
	TriangleMesh mesh(model.getVerticesFlatArray(),
		model.getNormalsFlatArray(),
		model.getVertexCount(),
		model.getFacesIndicesFlatArray(),
		model.getFacesCount(),mat);
	m_displayWidget->setTriangleMesh(mesh.getVerticesArray(),
		mesh.getNormalsArray(),
		mesh.getVertexCount(),
		(unsigned int*)mesh.getIndicesArray(),
		mesh.getIndexCount());
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

void ClothModelingDemo::initCloth()
{

	numVerts = meshSize * meshSize;

	// there are springs going right for each vertex
	// not on right edge, springs going down for each vertex
	// not on bottom edge
	numSprings = (meshSize - 1) * meshSize * 2;

	// cross springs
	numSprings += (meshSize - 1) * (meshSize - 1) * 2;

	// skipping springs
	numSprings += (meshSize - 2) * meshSize * 2;

	oldVertices.resize(numVerts);
	updatedVertices.resize(numVerts);
	springs.resize(numSprings);

	for (int i = 0; i < meshSize; i++) {
		for (int j = 0; j < meshSize; j++) {
			SpringVertex & v = oldVertices[i * meshSize + j];
			Point3f p = { float(j) - float(meshSize - 1) / 2, 10, float(i) - float(meshSize - 1) / 2 };
			v.pos = p;
			v.vel = { 0,0,0 };
			v.mass = mass;
			v.fixed = false;
			v.normal = { 0,0,0 };
		}
	}

	// fix four corners
	oldVertices[0].fixed = true;
	oldVertices[meshSize - 1].fixed = true;
	oldVertices[meshSize * (meshSize - 1)].fixed = true;
	oldVertices[meshSize * meshSize - 1].fixed = true;

	for (int i = 0; i < numVerts; i++) {
		updatedVertices[i] = oldVertices[i];
	}

	// init springs
	int id = 0;

	for (int i = 0; i < meshSize; i++) {
		for (int j = 0; j < meshSize - 1; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = i * meshSize + j + 1;

			springs[id].k = springConst;
			springs[id].x = length;
			++id;
		}
	}

	for (int i = 0; i < meshSize - 1; i++) {
		for (int j = 0; j < meshSize; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = (i + 1) * meshSize + j;
			springs[id].k = springConst;
			springs[id].x = length;
			id++;
		}
	}

	// cross springs
	for (int i = 0; i < meshSize - 1; i++) {
		for (int j = 0; j < meshSize - 1; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = (i + 1) * meshSize + j + 1;
			springs[id].k = springConst;
			springs[id].x = length * sqrt(2.0f);
			id++;
		}
	}

	for (int i = 0; i < meshSize - 1; i++) {
		for (int j = 1; j < meshSize; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = (i + 1) * meshSize + j - 1;
			springs[id].k = springConst;
			springs[id].x = length * sqrt(2.0f);
			id++;
		}
	}

	// skip springs
	for (int i = 0; i < meshSize; i++) {
		for (int j = 0; j < meshSize - 2; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = i * meshSize + j + 2;
			springs[id].k = springConst;
			springs[id].x = length * 2;
			id++;
		}
	}

	for (int i = 0; i < meshSize - 2; i++) {
		for (int j = 0; j < meshSize; j++) {
			springs[id].p1 = i * meshSize + j;
			springs[id].p2 = (i + 2) * meshSize + j;

			springs[id].k = springConst;
			springs[id].x = length * 2;
			id++;
		}
	}
}

void ClothModelingDemo::simulate()
{
	int currTime; //= glutGet(GLUT_ELAPSED_TIME);
	int msTimeSinceLastFrame = currTime - prevTime;
	float timeSinceLastFrame = msTimeSinceLastFrame / 1000.0;
	int elapsedTime = currTime - startTime;

	double timeSinceLastUpdate = 0.0;
	timeSinceLastUpdate += msTimeSinceLastFrame;

	bool updated = false;

	while (timeSinceLastUpdate > 10.0) {
		timeSinceLastUpdate -= 10.0;
		float timePassedInSeconds = 0.005f;
		updated = true;



		//evaluate tension for all springs
		for (int i = 0; i < numSprings; i++) {
			float length = (oldVertices[springs[i].p1].pos - oldVertices[springs[i].p2].pos).length();
			float dis = length - springs[i].x;
			springs[i].tension = springs[i].k * (dis / springs[i].x);
		}

		//
		for (int i = 0; i < numVerts; i++) {
			updatedVertices[i].fixed = oldVertices[i].fixed;
			updatedVertices[i].mass = oldVertices[i].mass;

			//set the velocity of fixed point as 0 
			if (oldVertices[i].fixed) {

				updatedVertices[i].pos = oldVertices[i].pos;
				updatedVertices[i].vel = { 0,0,0 };
			}
			else {
				Point3f pi = oldVertices[i].pos;
				Vector3f vi = oldVertices[i].vel;

				//state st;
				//st.x = pi; st.v = vi;
				Point3f x = pi;
				Vector3f v = vi;

				Vector3f forces = gravity;
				for (int j = 0; j < numSprings; j++) {
					Vector3f dir = { 0, 0, 0 };
					if (springs[j].p1 == i) {
						dir = (oldVertices[springs[j].p2].pos - pi).normalized();
					}
					else if (springs[j].p2 == i) {
						dir = (oldVertices[springs[j].p1].pos - pi).normalized();
					}
					forces = forces + springs[j].tension * dir;
				}

				Vector3f acc = forces / oldVertices[i].mass;


				if (euler) {
					integrateEuler(&x,&v, acc, timePassedInSeconds);
				}
				else {
					integrateRK4(&x,&v, acc, timePassedInSeconds);
				}
				///TODO:: Collision

				Ray aRay(v,x);
				Float t;
				if(m_scene->intersect(aRay,&t,nullptr) == true)
				{
					if(t <0.001)
					{
						updatedVertices[i].fixed = true;
					}
				}

				updatedVertices[i].pos = x;
				updatedVertices[i].vel = v;
			}
		}

		for (int i = 0; i < numVerts; i++) {
			oldVertices[i] = updatedVertices[i];
		}
	}

	if (updated) {
		for (int i = 0; i < numVerts; i++) 
			oldVertices[i].normal = { 0,0,0 };

		for (int i = 0; i < meshSize - 1; i++) {
			for (int j = 0; j < meshSize - 1; j++) {
				Point3f p0 = oldVertices[i*meshSize + j].pos;
				Point3f p1 = oldVertices[i*meshSize + j + 1].pos;
				Point3f p2 = oldVertices[(i + 1)*meshSize + j].pos;
				Point3f p3 = oldVertices[(i + 1)*meshSize + j + 1].pos;

				Vector3f tempNormal = Vector3f::crossProduct((p1 - p0), (p2 - p0));
				Vector3f tempNormal2 = Vector3f::crossProduct((p1 - p2), (p3 - p2));

				oldVertices[i*meshSize + j].normal = oldVertices[i*meshSize + j].normal + tempNormal;
				oldVertices[i*meshSize + j + 1].normal = oldVertices[i*meshSize + j + 1].normal + tempNormal + tempNormal2;
				oldVertices[(i + 1)*meshSize + j].normal = oldVertices[(i + 1)*meshSize + j].normal + tempNormal + tempNormal2;
				oldVertices[(i + 1)*meshSize + j + 1].normal = oldVertices[(i + 1)*meshSize + j + 1].normal + tempNormal2;
			}
		}

		for (int i = 0; i < numVerts; i++) {
			oldVertices[i].normal = oldVertices[i].normal.normalized();
		}
	}

	prevTime = currTime;
}




ClothModelingDemo::ClothModelingDemo(QWidget* parent):BaseDemoWidget(parent)
{
	//display widget intialization
	m_displayWidget = new OpenGLWidget(Camera(),this);
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
	connect(m_elasticity,SIGNAL(valueChanged(int)),this,SLOT(onSliderChanged(int)));
	connect(m_reset, SIGNAL(clicked()), this, SLOT(onResetButton()));
	initTestModel();
}

void ClothModelingDemo::onSliderChanged(int value)
{
	qDebug() << value;
}

void ClothModelingDemo::onResetButton()
{
	qDebug() << "Reset";
}

