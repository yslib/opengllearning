#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <qmath.h>

#include "core.h"
#include "PathTracingDemo.h"
#include "camera.h"

class QOpenGLShader;
class QOpenGLShaderProgram;
class QTimer;

class OpenGLWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(const Camera & cam = Camera(),QWidget * parent = nullptr)noexcept;
    void setAnimation(bool enable);
    void updateModel(const QVector<QVector3D> & vertices,const QVector<QVector3D> & normals);
    void setTriangleMesh(const Point3f * vertices,
        const Point3f * normals,
        int nVertex,
        const unsigned int * faceIndices,
        int nFaceIndex);

    //void setTriangleMesh(const TriangleMesh & mesh);
   
    //void setTriangleMesh(const TriangleMesh & mesh);
    Trans3DMat getPerspectiveMat()const;

    inline float aspectRatio()
    {
        return width() / static_cast<float>(height());
    }

    inline float verticalAngle()const{return m_verticalAngle;}
    inline const Camera & getCamera()const{return m_camera;}
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

    void paintModel();

private:

    bool m_drawWithIBO;
    int m_nVertex;
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

    //perspective parameter

    //event state varibles
    bool m_mousePressed;
    int m_prevXPos;
    int m_prevYPos;


    //transform matrix
    Trans3DMat m_projection;
    Trans3DMat m_model;
    Trans3DMat m_view;


    int m_projectAttriLocation;
    int m_modelAttriLocation;
    int m_viewAttriLocation;

    //opengl resources varibles
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ibo;

    QOpenGLShader * m_vshader;
    QOpenGLShader * m_fshader;
    QOpenGLShaderProgram * m_program;


    //model
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_norms;


    int m_attriPos;

};




#endif // OPENGLWINDOW_H
