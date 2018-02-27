#include "basedemowidget.h"
#include <QLayout>
BaseDemoWidget::BaseDemoWidget(QWidget *parent):QWidget(parent),m_displayWidget(nullptr),m_controlWidget(nullptr)
{
    m_layout = new QGridLayout;
    setLayout(m_layout);
}

void BaseDemoWidget::setDisplayWidget(QWidget *widget)
{
    m_displayWidget = widget;
    m_layout->addWidget(m_displayWidget,0,0);
}

void BaseDemoWidget::setControlWidget(QWidget *widget)
{
    m_controlWidget = widget;
    m_layout->addWidget(m_controlWidget,0,1);
}
