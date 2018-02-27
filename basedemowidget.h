#ifndef ABSTRACTDEMOWIDGET_H
#define ABSTRACTDEMOWIDGET_H
#include <QWidget>

class QGridLayout;

class BaseDemoWidget:public QWidget
{
    Q_OBJECT
public:
    explicit BaseDemoWidget(QWidget * parent = nullptr);
    QWidget * displayWidget()const{return m_displayWidget;}
    QWidget * controlWidget()const{return m_controlWidget;}
    virtual ~BaseDemoWidget(){}
protected:
    void setDisplayWidget(QWidget * widget);
    void setControlWidget(QWidget * widget);
private:
    QWidget * m_displayWidget;
    QWidget * m_controlWidget;
    QGridLayout * m_layout;
};
#endif // ABSTRACTDEMOWIDGET_H
