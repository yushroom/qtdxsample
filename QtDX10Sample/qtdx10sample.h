#ifndef QTDX10SAMPLE_H
#define QTDX10SAMPLE_H

#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include "ui_qtdx10sample.h"

class QtDX10Sample : public QMainWindow
{
	Q_OBJECT

public:
	QtDX10Sample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDX10Sample();

	void setVisible(bool);

private slots:
	void	idle();

private:
	//! Timer
	QTimer	m_timer;

	//! Elapsed Time
	float	m_fTime;

private:
	Ui::QtDX10SampleClass ui;
};

#endif // QTDX10SAMPLE_H
