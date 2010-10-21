#ifndef QTDX09SAMPLE_H
#define QTDX09SAMPLE_H

#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include "ui_qtdx09sample.h"

class QtDX09Sample : public QMainWindow
{
	Q_OBJECT

public:
	QtDX09Sample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDX09Sample();

	void setVisible(bool);

private slots:
	void	idle();

private:
	//! Timer
	QTimer	m_timer;

	//! Elapsed Time
	float	m_fTime;

	Ui::QtDX09SampleClass ui;
};

#endif // QTDX09SAMPLE_H
