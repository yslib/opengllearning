#ifndef ABSTRACTDEMOWIDGET_H
#define ABSTRACTDEMOWIDGET_H
#include <QWidget>


class QSpinBox;
class QSlider;
class QLabel;
class QGridLayout;

class TitledSliderWithSpinBox :public QWidget
{
	Q_OBJECT
public:
	explicit TitledSliderWithSpinBox(QWidget * parent = nullptr, const QString & title = QString(), Qt::Orientation orientation = Qt::Horizontal);
	void setMinimum(int value);
	void setMaximum(int value);
	void setRange(int min, int max);
	int minimum()const;
	int maximum()const;
	int value()const;
	void setEnabled(bool enabled);
	void setOrientation(Qt::Orientation orientation);

	public slots:
	void setValue(int value);
signals:
	void valueChanged(int value);
private:
	void createConnections();
	void createLayout(Qt::Orientation orientation = Qt::Horizontal);
private:
	QSpinBox * m_spinBox;
	QSlider * m_slider;
	QLabel * m_label;
	QGridLayout * m_layout;
};


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
