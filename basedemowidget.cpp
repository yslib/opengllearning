#include "basedemowidget.h"
#include <QLayout>
BaseDemoWidget::BaseDemoWidget(QWidget *parent):QWidget(parent),m_displayWidget(nullptr),m_controlWidget(nullptr)
{
    m_layout = new QGridLayout;
	m_layout->setColumnStretch(0, 7);
	m_layout->setColumnStretch(1, 3);
    setLayout(m_layout);
}

void BaseDemoWidget::setDisplayWidget(QWidget *widget)
{
    m_displayWidget = widget;
	//m_displayWidget->setMinimumWidth(500);
    m_layout->addWidget(m_displayWidget,0,0);
}

void BaseDemoWidget::setControlWidget(QWidget *widget)
{
    m_controlWidget = widget;
	//m_controlWidget->setMinimumWidth(300);
    m_layout->addWidget(m_controlWidget,0,1);
}
