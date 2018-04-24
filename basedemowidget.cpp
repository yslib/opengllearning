#include "basedemowidget.h"
#include <QLayout>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

TitledSliderWithSpinBox::TitledSliderWithSpinBox(QWidget *parent, const QString &title, Qt::Orientation orientation) :QWidget(parent)
{
	m_label = new QLabel(title, this);
	m_slider = new QSlider(orientation, this);
	m_spinBox = new QSpinBox(this);
	m_layout = new QGridLayout(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_layout->addWidget(m_label, 0, 0);
	createLayout(orientation);
	createConnections();
}

void TitledSliderWithSpinBox::setMinimum(int value)
{
	m_slider->setMinimum(value);
	m_spinBox->setMinimum(value);
}


void TitledSliderWithSpinBox::setMaximum(int value)
{
	m_slider->setMaximum(value);
	m_spinBox->setMaximum(value);
}


void TitledSliderWithSpinBox::setRange(int min, int max)
{
	m_slider->setRange(min, max);
	m_spinBox->setRange(min, max);
}

int TitledSliderWithSpinBox::minimum() const
{
	return m_slider->minimum();
}

int TitledSliderWithSpinBox::maximum() const
{
	return m_slider->maximum();
}

void TitledSliderWithSpinBox::setValue(int value)
{
	m_slider->setValue(value);
	m_spinBox->setValue(value);
}
int TitledSliderWithSpinBox::value() const
{
	return m_slider->value();
}

void TitledSliderWithSpinBox::setEnabled(bool enabled)
{
	m_slider->setEnabled(enabled);
	m_spinBox->setEnabled(enabled);
}

void TitledSliderWithSpinBox::setOrientation(Qt::Orientation orientation)
{
	createLayout(orientation);
}

void TitledSliderWithSpinBox::createConnections()
{
	connect(m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	connect(m_slider, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));
}

void TitledSliderWithSpinBox::createLayout(Qt::Orientation orientation)
{
	m_layout->removeWidget(m_slider);
	m_layout->removeWidget(m_spinBox);
	if (orientation == Qt::Vertical) {
		m_layout->addWidget(m_slider, 1, 0);
		m_layout->addWidget(m_spinBox, 2, 0);
	}
	else {
		m_layout->addWidget(m_slider, 0, 1);
		m_layout->addWidget(m_spinBox, 0, 2);
	}
}





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
