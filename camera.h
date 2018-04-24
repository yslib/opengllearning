#ifndef CAMERA_H
#define CAMERA_H

#include "core.h"
#include "qmath.h"
#include <QDebug>






class AbstractCamera {
public:
    virtual const Trans3DMat & getViewMat()const = 0;
    virtual const QVector3D & getPosition()const = 0;
        virtual const Trans3DMat & getPerspectiveMat()const = 0;
private:

};



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement {
    Forward,
    Backward,
    Left,
    Right
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

    QVector3D front()const{
        return Front;
    }
    QVector3D right()const{
        return Right;
    }
    QVector3D up()const{
        return Up;
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Trans3DMat view()
    {
        Trans3DMat view;
        view.setToIdentity();
        view.lookAt(Position, Position + Front, Up);
        return view;
    }
    QVector3D position()const{
        return Position;
    }
    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == CameraMovement::Forward)
            Position += Front * velocity;
        if (direction == CameraMovement::Backward)
            Position -= Front * velocity;
        if (direction == CameraMovement::Left)
            Position -= Right * velocity;
        if (direction == CameraMovement::Right)
            Position += Right * velocity;
    }
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
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

inline
QDebug operator<<(QDebug db, const Camera & cam) {
	db << "{Pos:" << cam.position() << ",Up:" << cam.up() << ",Yaw:" << cam.Yaw << ",Pitch:" << cam.Pitch;
	return db;
}


#endif // CAMERA_H
