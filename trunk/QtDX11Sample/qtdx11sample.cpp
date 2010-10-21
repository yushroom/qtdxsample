#include "qtdx11sample.h"
#include "../common/dxwidget.h"

QtDX11Sample::QtDX11Sample(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	m_timer.setParent( this );
	m_timer.setInterval( 30 ) ; // in msec 
	m_timer.setSingleShot( false ) ; 
	QObject::connect( &m_timer, SIGNAL( timeout() ), this, SLOT( idle() ) ) ; 
	m_fTime = 0.0f;
}

QtDX11Sample::~QtDX11Sample()
{

}

void QtDX11Sample::setVisible(bool visible)
{
	if(visible)
	{
		QWidget::setVisible(visible);
		m_timer.start();
	}
	else
	{
		m_timer.stop();
		QWidget::setVisible(visible);
	}
}

void QtDX11Sample::idle()
{
	m_fTime += m_timer.interval()/1000.0f;
	//! DirectX Widget
	DXWidget*	widget = (DXWidget *)this->centralWidget();
	if(widget)
		widget->render( m_fTime );
}
