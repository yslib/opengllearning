#pragma once
#ifndef _PATHTRACINGDEMO_H_
#define _PATHTRACINGDEMO_H_
#include "basedemowidget.h"

class PathTracingDemo :
	public BaseDemoWidget
{
public:
	PathTracingDemo();

	virtual ~PathTracingDemo();
private:
	QImage * m_displayWidget;
	QWidget * m_controlWidget;

};

#endif