#ifndef QTDXSAMPLE_H
#define QTDXSAMPLE_H

#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include "ui_qtdxsample.h"

class QtDXSample : public QMainWindow
{
	Q_OBJECT

public:
	QtDXSample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDXSample();

	void	setVisible(bool);

private slots:
	void	idle();
	void	toggleAnimation( bool pressed );

private:
	//! Timer
	QTimer	m_timer;

	//! Elapsed Time
	float	m_fTime;

	Ui::QtDXSampleClass ui;
};

#endif // QTDXSAMPLE_H
