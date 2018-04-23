#ifndef CLOTHMODELINGDEMO_H
#define CLOTHMODELINGDEMO_H
#include "basedemowidget.h"

#include "geometry.h"
#include <memory>

class OpenGLWidget;
class QSlider;
class TitledSliderWithSpinBox;
class QComboBox;
class QPushButton;
class Scene;
class BVHTreeAccelerator;

struct SpringVertex {
	Point3f pos;
	Vector3f vel;
	float mass;
	bool fixed;
	Vector3f normal;
};

struct Spring {
	int p1; // endpoints
	int p2;
	float tension;
	float k;
	float x;
};


class ClothModelingDemo:public BaseDemoWidget
{
	
	Q_OBJECT
	OpenGLWidget * m_displayWidget;
	TitledSliderWithSpinBox * m_elasticity;
	QComboBox * m_integrationMethod;
	QPushButton * m_reset;

	std::shared_ptr<Scene> m_scene;
	std::shared_ptr<BVHTreeAccelerator> m_aggregate;

	std::vector<SpringVertex> oldVertices;
	std::vector<SpringVertex> updatedVertices;
	std::vector<Spring> springs;

	static const int meshSize = 20;
	int numVerts;
	int numSprings;

	Float mass = 0.01;
	Float length = 1.0;
	Float springConst = 12.0f;
	Vector3f gravity = { 0,0.98,0 };
	int prevTime;
	int startTime;
	bool euler;

private:		/*Private Member Functions*/
	void initTestModel();
	void initCloth();
	void simulate();

public:
    ClothModelingDemo(QWidget * parent = nullptr);
public slots:
	void onResetButton();
	void onSliderChanged(int value);
};

#endif // CLOTHMODELINGDEMO_H