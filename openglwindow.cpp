#include "openglwindow.h"
#include <QDebug>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>
#include <QTimer>

OpenGLWidget::OpenGLWidget(const Camera & cam,QWidget *parent) :
	QOpenGLWidget(parent),
	m_vshader(0),
	m_fshader(0),
	m_program(0),
	m_mousePressed(false),
	m_eye(QVector3D(0, 0, 1)),
	m_center(QVector3D(0, 0, 0)),
	m_up(QVector3D(0, 1, 0)),
	m_lightPos(QVector3D(0, 10, 0)),
	m_lightColor(QVector3D(1.0f, 1.0f, 1.0f)),
    m_objectColor(QVector3D(.7f, 0.8f, 0.7f)),
	m_verticalAngle(45.f),
        m_camera(cam){
	QSurfaceFormat fmt;
	fmt.setDepthBufferSize(24);
	fmt.setStencilBufferSize(8);
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);
	setFormat(fmt);
	setFocusPolicy(Qt::StrongFocus);
	m_modelUpdated = false;
	//setMinimumSize(500, 500);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//matrix initialization
	m_model.setToIdentity();
	m_view.lookAt(m_eye, m_center, m_up),
	m_projection.perspective(m_verticalAngle, aspectRatio(), 0.01f, 100.0f);

	m_timer = new QTimer(this);
	m_timer->setInterval(10);
	connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));


}
OpenGLWidget::~OpenGLWidget()
{
	makeCurrent();
	//delete m_program;
	//delete m_vshader;
	//delete m_fshader;
	//m_vbo.destroy();
	//m_vao.destroy();
	doneCurrent();
}

void OpenGLWidget::initializeGL()
{

	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(.2f, .3f, .3f, 1.0f);

	//Initialized program shader
	m_vshader = new QOpenGLShader(QOpenGLShader::Vertex);
	
        m_vshader->compileSourceFile(":new/shaders/phongshadingvertexshader.glsl");
	m_fshader = new QOpenGLShader(QOpenGLShader::Fragment);
        m_fshader->compileSourceFile(":new/shaders/phongshadingfragmentshader.glsl");
	m_program = new QOpenGLShaderProgram();
	m_program->addShader(m_vshader);
	m_program->addShader(m_fshader);
	m_program->link();
	m_program->bind();
	//create VAO
	m_vao.create();
	m_vao.bind();


	//create VBO
	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	int totalBytes = m_vertices.count() * sizeof(GLfloat) * 3 + m_normals.count() * sizeof(GLfloat) * 3;
	m_vbo.allocate(totalBytes);
	m_vbo.write(0,m_vertices.constData(), m_vertices.count() * 3 * sizeof(float));
	m_vbo.write(m_vertices.count() * 3 * sizeof(GLfloat), m_normals.constData(), m_normals.count() * 3 * sizeof(GLfloat));


	m_program->enableAttributeArray(0);
	m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	m_program->enableAttributeArray(1);
	m_program->setAttributeBuffer(1, GL_FLOAT, m_vertices.size()*3*sizeof(GLfloat),3,0);

	//m_vbo.release();
	//m_vao.release();
	m_program->release();

}
void OpenGLWidget::resizeGL(int w, int h) {

	//Updating m_projection matrix here
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);

}
void OpenGLWidget::paintGL() {
	//glClearColor(.2f,.3f,.3f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	if (m_mousePressed == true) {
		//update matrix here
		int deltaX = event->x() - m_prevXPos;
		int deltaY = m_prevYPos - event->y();
		m_prevXPos = event->x();
		m_prevYPos = event->y();
		//updateCameraVectors(deltaX, deltaY);
		m_camera.ProcessMouseMovement(deltaX, deltaY);
	}


}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_mousePressed = false;
}

void OpenGLWidget::keyPressEvent(QKeyEvent * event)
{
	switch (event->key())
	{
	case Qt::Key_W:
		m_camera.ProcessKeyboard(CameraMovement::Forward, 0.1);
		break;
	case Qt::Key_A:
		m_camera.ProcessKeyboard(CameraMovement::Left, 0.1);
		break;
	case Qt::Key_S:
		m_camera.ProcessKeyboard(CameraMovement::Backward, 0.1);
		break;
	case Qt::Key_D:
		m_camera.ProcessKeyboard(CameraMovement::Right, 0.1);
		break;
	default:
		break;
	}
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
	//updateCameraVectors(0, 0, event->angleDelta().y());
}

void OpenGLWidget::updateCameraVectors(int deltaX, int deltaY, int deltaWheel)
{
	float eyeLength = m_eye.length();
	QVector3D d = 0.01*static_cast<float>(deltaWheel)*m_eye.normalized();
	if (eyeLength < 2.0 && deltaWheel > 0) {
		m_eye += d;
	}
	else if (eyeLength > 20.0 && deltaWheel < 0) {
		m_eye += d;
	}
	else if (eyeLength >= 2.0 && eyeLength < 20.0) {
		m_eye += d;
	}

	//rotatation
	Trans3DMat rotate;
	rotate.setToIdentity();
	rotate.rotate(0.2*deltaX, QVector3D(0, 1, 0));
	m_eye = m_eye * rotate;

	QVector3D direction = m_center - m_eye;
	QVector3D right = QVector3D::crossProduct(direction, m_up);
	rotate.setToIdentity();
	rotate.rotate(0.2*deltaY, right);
	qDebug() << deltaY;
	m_eye = m_eye * rotate;
	m_up = QVector3D::crossProduct(right, direction);
	m_view.setToIdentity();
	m_view.lookAt(m_eye, m_center, m_up);
}

float OpenGLWidget::aspectRatio()
{
	return width() / static_cast<float>(height());
}

void OpenGLWidget::paintModel()
{
	if (m_modelUpdated == true) {
		m_modelUpdated = false;
	}
	else {
		m_program->bind();
		{
			m_vao.bind();
			m_program->setUniformValue("projection_matrix",m_projection);
			m_program->setUniformValue("view_matrix", m_camera.view());
			m_program->setUniformValue("model_matrix", m_model);
			m_program->setUniformValue("light_pos", m_camera.Position);
			m_program->setUniformValue("light_color", m_lightColor);
			m_program->setUniformValue("view_pos", m_camera.Position);
			m_program->setUniformValue("object_color", m_objectColor);
			glDrawArrays(GL_TRIANGLES, 0, m_vertices.count());
			m_vao.release();
		}
		m_program->release();
	}
}

void OpenGLWidget::setAnimation(bool enable)
{
	if (enable == true) {
		m_timer->start();
	}
	else {
		m_timer->stop();
	}
}



void OpenGLWidget::updateModel(const QVector<QVector3D> &vertices,const QVector<QVector3D> & normals)
{
	m_vertices = vertices;
	m_normals = normals;
	if (isValid() == false)
		return;
	makeCurrent();
	m_vao.bind();
	m_vbo.bind();
	m_program->bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	int totalBytes = vertices.count() * 3 * sizeof(GLfloat) + m_normals.count() * 3 * sizeof(GLfloat);
	m_vbo.allocate(totalBytes);
	//m_vbo.allocate(m_vertices.constData(), m_vertices.count() * 3 * sizeof(float));
	m_vbo.write(0, vertices.constData(), vertices.count() * 3*sizeof(GLfloat));
	m_vbo.write(vertices.count() * sizeof(GLfloat) * 3, normals.constData(), normals.count() * 3 * sizeof(GLfloat));
	
	
	m_program->enableAttributeArray(0);
	m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	m_program->enableAttributeArray(1);
	m_program->setAttributeBuffer(1, GL_FLOAT, vertices.size() * 3 * sizeof(GLfloat), 3, 0);
	m_program->release();
	m_vbo.release();
	m_vao.release();
    doneCurrent();
}

void OpenGLWidget::updateModel(const TriangleMesh &mesh)
{

}

Trans3DMat OpenGLWidget::getPerspectiveMat() const
{
    return m_projection;
}
