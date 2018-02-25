#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>

class QOpenGLShader;
class QOpenGLShaderProgram;
class QTimer;

class OpenGLWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget * parent = nullptr);
    void setAnimation(bool enable);
    void updateModel(const QVector<QVector3D> & model);
    ~OpenGLWidget();
protected:
    void initializeGL()override;
    void resizeGL(int w, int h)override;
    void paintGL()override;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
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
    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;

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


    int m_attriPos;

};

#endif // OPENGLWINDOW_H
