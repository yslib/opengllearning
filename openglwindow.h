#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <qmath.h>


class AbstractCamera {
public:
	AbstractCamera();
	virtual QMatrix4x4 getViewMatrix()const = 0;
	virtual QVector3D getPosition()const = 0;
private:

};



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	QVector3D Position;
	QVector3D Front;
	QVector3D Up;
	QVector3D Right;
	QVector3D WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(Position), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = QVector3D(posX, posY, posZ);
		WorldUp = QVector3D(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	QMatrix4x4 GetViewMatrix()
	{
		QMatrix4x4 view;
		view.setToIdentity();
		view.lookAt(Position, Position + Front, Up);
		return view;
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == Camera_Movement::FORWARD)
			Position += Front * velocity;
		if (direction == Camera_Movement::BACKWARD)
			Position -= Front * velocity;
		if (direction == Camera_Movement::LEFT)
			Position -= Right * velocity;
		if (direction == Camera_Movement::RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		QVector3D front;
		front.setX(cos(qDegreesToRadians(Yaw)) * cos(qDegreesToRadians(Pitch)));
		front.setY( sin(qDegreesToRadians(Pitch)));
		front.setZ (sin(qDegreesToRadians(Yaw)) * cos(qDegreesToRadians(Pitch)));
		Front = front.normalized();
		// Also re-calculate the Right and Up vector
		Right = QVector3D::crossProduct(Front, WorldUp).normalized();
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = QVector3D::crossProduct(Right, Front).normalized();
	}
};

class QOpenGLShader;
class QOpenGLShaderProgram;
class QTimer;

class OpenGLWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget * parent = nullptr);
    void setAnimation(bool enable);
    void updateModel(const QVector<QVector3D> & vertices,const QVector<QVector3D> & normals);
    ~OpenGLWidget();
protected:
    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
	void keyPressEvent(QKeyEvent *event)override;
    void wheelEvent(QWheelEvent *event)override;

private:
    void updateCameraVectors(int deltaX = 0,int deltaY = 0,int deltaWheel = 0);
    float aspectRatio();

    void paintModel();

private:
    //
    bool m_modelUpdated;
    QTimer* m_timer;

    //camera parameters
	Camera m_camera;


    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;

	QVector3D m_lightPos;
	QVector3D m_lightColor;
	QVector3D m_objectColor;

    float m_verticalAngle = 0;


    //event state varibles
    bool m_mousePressed;
    int m_prevXPos;
    int m_prevYPos;


    //transform matrix
    QMatrix4x4 m_projection;
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;


    int m_projectAttriLocation;
    int m_modelAttriLocation;
    int m_viewAttriLocation;

    //opengl resources varibles
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    QOpenGLShader * m_vshader;
    QOpenGLShader * m_fshader;
    QOpenGLShaderProgram * m_program;


    //model
    QVector<QVector3D> m_vertices;
	QVector<QVector3D> m_normals;


    int m_attriPos;

};




#endif // OPENGLWINDOW_H
