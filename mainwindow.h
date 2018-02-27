#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class QGridLayout;
class QLabel;
class QSlider;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QLineEdit;
class QGroupBox;
class MarchingCubesDemo;
class BaseDemoWidget;
class LoopSubdivisionSurfaceDemo;
class QMenu;
class QAction;
class QMenuBar;


class OpenGLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QGridLayout * m_mainLayout;
    MarchingCubesDemo * m_mcDemo;
	LoopSubdivisionSurfaceDemo * m_loopSubdivisionDemo;
	QMenu * m_modeMenu;
	QAction * m_marchingCubesDemoAction;
	QAction * m_loopSubdivisionDemoAction;
	QMenuBar * m_menuBar;
private:
    void setWidget(BaseDemoWidget * widget);
	private slots:
	void onMenuActions(QAction * action);
};

#endif // MAINWINDOW_H
