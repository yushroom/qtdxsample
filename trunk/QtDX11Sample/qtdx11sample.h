#ifndef QTDX11SAMPLE_H
#define QTDX11SAMPLE_H

#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include "ui_qtdx11sample.h"

class QtDX11Sample : public QMainWindow
{
	Q_OBJECT

public:
	QtDX11Sample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDX11Sample();

	void setVisible(bool);

private slots:
	void	idle();

private:
	//! Timer
	QTimer	m_timer;

	//! Elapsed Time
	float	m_fTime;

private:
	Ui::QtDX11SampleClass ui;
};

#endif // QTDX11SAMPLE_H
