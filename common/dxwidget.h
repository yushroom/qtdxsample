#pragma once

#include "common.h"

#include <QWidget>
#include <QResizeEvent>

class DXWidget : public QWidget
{
public:
	DXWidget( QWidget *parent = 0, Qt::WFlags flags = 0 ) : QWidget( parent, flags )
	{
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);

		m_standBy = false;
		m_lastRendered = 0;
	}
	virtual ~DXWidget()
	{
		uninitialize();
	}

	virtual HRESULT initialize() = 0;
	virtual void	uninitialize() {}

	virtual HRESULT	restoreDeviceObjects() = 0;
	virtual HRESULT invalidateDeviceObjects() = 0;

	virtual HRESULT	render( double fTime )
	{
		m_lastRendered = fTime;
		return S_OK;
	}
	virtual HRESULT	present()
	{
		return S_OK;
	}

protected:
	QPaintEngine *paintEngine() const { return 0; } 

	virtual void	paintEvent( QPaintEvent *paintE )
	{
		Q_UNUSED(paintE);
		present();
	}

	//! if stand-by mode
	bool	m_standBy;

	//! Last updated time
	double	m_lastRendered;

};
