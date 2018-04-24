#ifndef CLOTHMODELINGDEMO_H
#define CLOTHMODELINGDEMO_H
#include "basedemowidget.h"

#include "geometry.h"
#include <memory>
#include "core.h"

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

	std::vector<SpringVertex> m_oldVertices;
	std::vector<SpringVertex> m_updatedVertices;
	std::vector<Spring> m_springs;
	std::vector<Point3f> m_vertice;
	std::vector<Vector3f> m_normals;
	std::vector<int> m_indices;

	int m_meshSize;
	int m_nVerts;
	int m_nIndices;
	int m_nSprings;

	int m_nModelVerts;
	int m_nModelIndices;

	Float m_pointMass;
	Float m_springLength;
	Float m_springConst;
	Float m_damping;
	Vector3f m_gravity;

	int m_prevTime;
	int m_startTime;
	bool m_euler;

	QTimer * m_timer;


	bool m_once;

private:		/*Private Member Functions*/
	void initModel();
	void resetCloth();
	void simulate();

public:
    ClothModelingDemo(QWidget * parent = nullptr);
public slots:
	void onResetButton();
	void onSliderChanged(int value);
	void simulation();
};

#endif // CLOTHMODELINGDEMO_H