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
private:
    void setWidget(BaseDemoWidget * widget);
};

#endif // MAINWINDOW_H
