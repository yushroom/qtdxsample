#pragma once

#include "common.h"
#include "logging.h"

#include <QtCore/QPointer>
#include <QtGui/QMainWindow>
#include "ui_qtdxsample.h"

class DXWidget;
class QtDXSample : public QMainWindow
{
	Q_OBJECT

public:
	QtDXSample(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDXSample();

private slots:
	void	repaintCanvas();
	void	toggleAnimation(bool pressed);

private:
	QPointer<DXWidget>	widget;
	Ui::QtDXSampleClass ui;
};
