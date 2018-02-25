#include "openglwindow.h"
#include <QDebug>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>
#include <QTimer>

OpenGLWidget::OpenGLWidget(QWidget *parent):
    QOpenGLWidget(parent),
    m_vshader(0),
    m_fshader(0),
    m_program(0),
    m_mousePressed(false),
    m_eye(QVector3D(0,0,10)),
    m_center(QVector3D(0,0,0)),
    m_up(QVector3D(0,1,0)),
    m_verticalAngle(45.f){


    m_modelUpdated = false;
    setMinimumSize(300,300);
    //matrix initialization
    m_model.setToIdentity();
    m_view.lookAt(m_eye,m_center,m_up),
    m_projection.perspective(m_verticalAngle,aspectRatio(),0.01f,100.0f);

    m_timer = new QTimer(this);
    m_timer->setInterval(10);
    connect(m_timer,&QTimer::timeout,this,QOverload<>::of(&QWidget::update));


}
OpenGLWidget::~OpenGLWidget()
{
//    makeCurrent();
//    delete m_program;
//    delete m_vshader;
//    delete m_fshader;
//    m_vbo.destroy();
//    m_vao.destroy();
//    doneCurrent();
}

void OpenGLWidget::initializeGL()
{

   qDebug()<<"initializeGL()";
   initializeOpenGLFunctions();
   glEnable(GL_DEPTH_TEST);

   glClearColor(.2f,.3f,.3f,1.0f);

    //Initialized program shader
   m_vshader = new QOpenGLShader(QOpenGLShader::Vertex);
   const char * vcode =
          "#version 330 core \
           layout (location = 0) in vec3 aPos;\
           layout (location = 1) in vec3 aNor; \
           uniform mat4 model; \
           uniform mat4 view; \
           uniform mat4 projection;\
           out vec3 Normal;\
           out vec3 FragPos;\
           void main() \
           {            \
               gl_Position = projection*view*model*vec4(aPos,1.0);\
               Normal = aNor;\
               FragPos = vec3(model*vec4(aPos,1.0));\
           }";
   m_vshader->compileSourceCode(vcode);

   m_fshader = new QOpenGLShader(QOpenGLShader::Fragment);
   const char * fcode =
          "#version 330 core \
           out vec4 FragColor;  \
           in vec3 Normal;\
           in vec3 FragPos;\
           void main()\
           { \
               FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\
           } ";

   m_fshader->compileSourceCode(fcode);
   m_program = new QOpenGLShaderProgram();
   m_program->addShader(m_vshader);
   m_program->addShader(m_fshader);
   m_program->link();
   m_program->bind();

   m_attriPos = m_program->attributeLocation("aPos");
//   m_modelAttriLocation = m_program->attributeLocation("model");
//   m_viewAttriLocation = m_program->attributeLocation("view");
//   m_projectAttriLocation = m_program->attributeLocation("projection");

//    m_vertices<<QVector3D(-0.5f,-0.5f,0.0f)
//           <<QVector3D(0.5f,-0.5f,0.0f)
//          <<QVector3D(0.0f,0.5f,0.0f)
//         <<QVector3D(-0.5f,0.5f,0.0f)
//        <<QVector3D(0.5f,0.5f,0.0f)
//       <<QVector3D(0.0f,-0.5f,0.0f);



     //create VAO
    m_vao.create();
    m_vao.bind();


     //create VBO
    m_vbo.create();
    qDebug()<<m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_vbo.allocate(m_vertices.constData(),m_vertices.count()*3*sizeof(float));

    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0,GL_FLOAT,0,3);

    //m_vbo.release();
    //m_vao.release();
    m_program->release();

}
void OpenGLWidget::resizeGL(int w, int h){

    //Updating m_projection matrix here
    m_projection.setToIdentity();
    m_projection.perspective(45.0f,w/float(h),0.01f,100.0f);

}
void OpenGLWidget::paintGL(){
    //glClearColor(.2f,.3f,.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//    m_program->bind();
//    {
//        m_program->setUniformValue("projection",m_projection);
//        m_program->setUniformValue("view",m_view);
//        m_program->setUniformValue("model",m_model);
//        m_vao.bind();
//        glDrawArrays(GL_TRIANGLES,0,3);
//        m_vao.release();
//    }
//    m_program->release();
    paintModel();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    m_prevXPos = event->x();
    m_prevYPos = event->y();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mousePressed == true){
        //update matrix here
        int deltaX = event->x()-m_prevXPos;
        int deltaY = event->y()-m_prevXPos;
        updateCameraVectors(deltaX,deltaY);
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
   m_mousePressed = false;
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
   updateCameraVectors(0,0,event->angleDelta().y());
}

void OpenGLWidget::updateCameraVectors(int deltaX, int deltaY, int deltaWheel)
{
     float eyeLength = m_eye.length();
     QVector3D d = 0.01*static_cast<float>(deltaWheel)*m_eye.normalized();
    if(eyeLength < 2.0 && deltaWheel > 0){
        m_eye+=d;
    }else if(eyeLength >20.0 && deltaWheel < 0){
        m_eye += d;
    }else if(eyeLength >= 2.0 && eyeLength < 20.0){
        m_eye += d;
    }

    //rotatation
    QMatrix4x4 rotate;
    rotate.setToIdentity();
    rotate.rotate(0.2*deltaX,QVector3D(0,1,0));
    m_eye = m_eye*rotate;

    QVector3D direction = m_center-m_eye;
    QVector3D right = QVector3D::crossProduct(direction,m_up);
    rotate.setToIdentity();
    rotate.rotate(0.2*deltaY,right);
    qDebug()<<deltaY;
    m_eye = m_eye*rotate;
    m_up = QVector3D::crossProduct(right,direction);
    m_view.setToIdentity();
    m_view.lookAt(m_eye,m_center,m_up);
}

float OpenGLWidget::aspectRatio()
{
    return width()/static_cast<float>(height());
}

void OpenGLWidget::paintModel()
{
   if(m_modelUpdated == true){
       m_modelUpdated = false;
   }else{
       m_program->bind();
       {
           m_vao.bind();
           m_program->setUniformValue("projection",m_projection);
           m_program->setUniformValue("view",m_view);
           m_program->setUniformValue("model",m_model);
           glDrawArrays(GL_TRIANGLES,0,m_vertices.count());
           m_vao.release();
       }
       m_program->release();
   }
}

void OpenGLWidget::setAnimation(bool enable)
{
    if(enable == true){
        m_timer->start();
    }
    else {
        m_timer->stop();
    }
}



void OpenGLWidget::updateModel(const QVector<QVector3D> &model)
{
   m_vertices = model;
   if(isValid() == false)
       return;
   makeCurrent();
   m_vao.bind();
   m_vbo.bind();
   m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
   m_vbo.allocate(m_vertices.constData(),m_vertices.count()*3*sizeof(float));
   m_vbo.release();
   m_vao.release();
   doneCurrent();
}
