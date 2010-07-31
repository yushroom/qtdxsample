#pragma once

#include "common.h"

#include <QWidget.h>
#include <QtCore/QTimer>
#include <QResizeEvent>

class DXWidget : public QWidget
{
	Q_OBJECT

public:

	DXWidget( QWidget *parent = 0, Qt::WFlags flags = 0 );

	virtual ~DXWidget();

	virtual HRESULT initialize();
	virtual void	uninitialize();

	virtual HRESULT	restoreDeviceObjects();
	virtual HRESULT invalidateDeviceObjects();

	virtual HRESULT	render();

#ifdef USE_D3D
	virtual void	clearScene( D3DXCOLOR ClearColor, float Z=1.0f, DWORD Stencil=0 );
	virtual void	clearRenderTarget( D3DXCOLOR ClearColor );
	virtual void	clearDepthStencil( float Z=1.0f, DWORD Stencil=0 );

	virtual HRESULT	beginScene();
	virtual HRESULT	endScene();
	virtual HRESULT	present();
#endif

#ifdef USE_D2D
	virtual void	clearRenderTarget( D2D1::ColorF ClearColor );
	virtual void	beginDraw();
	virtual HRESULT	endDraw();
#endif

	virtual void	startTimer();
	virtual void	stopTimer();

public slots:
	void idle();

protected:
	QPaintEngine *paintEngine() const { return 0; } 

	virtual void	paintEvent( QPaintEvent *paintE );
	virtual void	resizeEvent( QResizeEvent *p_event );

	//! Timer
	QTimer	m_timer;

	//! Elapsed Time
	float	m_fTime;

	//! if stand-by mode
	bool	m_standBy;

#if USE_D3D==9
	//! Used to create the D3DDevice
	IDirect3D9*				m_pD3D;

	//! Our rendering device
	IDirect3DDevice9*		m_pDevice;

	//! D3D Device Parameterss
	D3DPRESENT_PARAMETERS	m_d3dpp;
#endif

#if USE_D3D==10
	//! Our rendering device
	ID3D10Device1*			m_pDevice;

	IDXGISwapChain*			m_pSwapChain;

	DXGI_SWAP_CHAIN_DESC	m_swapDesc;

	ID3D10RenderTargetView*	m_pRenderTargetView;

	ID3D10Texture2D*		m_pDepthStencil;

	ID3D10DepthStencilView*	m_pDepthStencilView;

	D3D10_FEATURE_LEVEL1	m_FeatureLevel;
#endif

#if USE_D3D==11
	//! Our rendering device
	ID3D11Device*			m_pDevice;

	ID3D11DeviceContext*	m_pDeviceContext;

	IDXGISwapChain*			m_pSwapChain;

	DXGI_SWAP_CHAIN_DESC	m_swapDesc;

	ID3D11RenderTargetView*	m_pRenderTargetView;

	ID3D11Texture2D*		m_pDepthStencil;

	ID3D11DepthStencilView*	m_pDepthStencilView;

	D3D_FEATURE_LEVEL       m_FeatureLevel;
#endif

#ifdef USE_D2D
	ID2D1Factory*			m_pD2DFactory;

	IWICImagingFactory*		m_pWICFactory;

	IDWriteFactory*			m_pDWriteFactory;

	ID2D1HwndRenderTarget*	m_pHwndRenderTarget;
#endif

};
