#ifndef ABSTRACTDEMOWIDGET_H
#define ABSTRACTDEMOWIDGET_H
#include <QWidget>

class BaseDemoWidget:public QWidget
{
    Q_OBJECT
public:
    explicit BaseDemoWidget(QWidget * parent = nullptr):QWidget(parent),m_displayWidget(nullptr),m_controlWidget(nullptr){}
    QWidget * displayWidget()const{return m_displayWidget;}
    QWidget * controlWidget()const{return m_controlWidget;}
    virtual ~BaseDemoWidget(){}
protected:
    void setDisplayWidget(QWidget * widget){m_displayWidget = widget;}
    void setControlWidget(QWidget * widget){m_controlWidget = widget;}
private:
    QWidget * m_displayWidget;
    QWidget * m_controlWidget;
};

#endif // ABSTRACTDEMOWIDGET_H
