#include "qtdxsample.h"
#include "qtdxwidget.h"

#if USE_D3D==9
#include "MyDX9Widget.h"
#endif

#if USE_D3D==10
#include "MyDX10Widget.h"
#endif


QtDXSample::QtDXSample(QWidget *parent, Qt::WFlags flags) :
	QMainWindow(parent, flags)
#if USE_D3D==9
	,widget(new MyDX9Widget())
#endif
#if USE_D3D==10
	,widget(new MyDX10Widget())
#endif
{
	ui.setupUi(this);

	if(widget)
	{
		setCentralWidget(widget);
		//widget->setParent(centralWidget());
		//widget->setFixedSize(300, 300);
		widget->initialize();
	}
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
