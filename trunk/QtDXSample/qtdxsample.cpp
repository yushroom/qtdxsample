#include "qtdxsample.h"
#include "qtdxwidget.hpp"

QtDXSample::QtDXSample(QWidget *parent, Qt::WFlags flags) :
	QMainWindow(parent, flags),
	widget(new DXWidget())
{
	ui.setupUi(this);

	widget->setParent(centralWidget());
    //setCentralWidget(widget);
	widget->setFixedSize(300, 300);
    widget->InitD3D();
}

QtDXSample::~QtDXSample()
{

}

void	QtDXSample::repaintCanvas()
{
	widget->render();
}

void	QtDXSample::toggleAnimation(bool pressed)
{
	if(pressed)
		widget->startTimer();
	else
		widget->stopTimer();
}
