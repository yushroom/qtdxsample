#include "qtdxsample.h"
#include "qtdxwidget.h"

#include <QMessageBox>

#if USE_D3D==9
#include "MyDX9Widget.h"
#endif

#if USE_D3D==10
#include "MyDX10Widget.h"
#endif

#if USE_D3D==11
#include "MyDX11Widget.h"
#endif

#if USE_D2D==1
#include "MyD2DWidget.h"
#endif


QtDXSample::QtDXSample(QWidget *parent, Qt::WFlags flags) :
	QMainWindow(parent, flags)
#if USE_D3D==9
	,widget(new MyDX9Widget())
#endif
#if USE_D3D==10
	,widget(new MyDX10Widget())
#endif
#if USE_D3D==11
	,widget(new MyDX11Widget())
#endif
#if USE_D2D==1
	,widget(new MyD2DWidget())
#endif
{
	ui.setupUi(this);

	if(widget)
	{
		setCentralWidget(widget);
		//widget->setParent(centralWidget());
		//widget->setFixedSize(300, 300);
		HRESULT hr;
		if( FAILED(hr=widget->initialize() ) )
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(QObject::tr("Failed to initialize DirectX"));
			QString msg(DXGetErrorDescriptionA(hr));
			msgBox.setText(msg);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();

			delete widget;
			widget = 0;
		}
	}
}

QtDXSample::~QtDXSample()
{

}

void	QtDXSample::repaintCanvas()
{
	if(widget)
		widget->render();
}

void	QtDXSample::toggleAnimation(bool pressed)
{
	if(!widget) return;

	if(pressed)
		widget->startTimer();
	else
		widget->stopTimer();
}
