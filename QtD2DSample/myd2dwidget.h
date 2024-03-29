#pragma once

#define USE_D2D 1
#include "../GUI/dxwidget.h"

class MyD2DWidget : public DXWidget
{
	//Q_OBJECT

public:
	MyD2DWidget( QWidget *parent = 0, Qt::WFlags flags = 0 )
		: DXWidget( parent, flags ), m_pD2DFactory(0)
	{
	}

	virtual ~MyD2DWidget()
	{
		uninitialize();
	}

	//-----------------------------------------------------------------------------
	// Name: restoreDeviceObjects()
	// Desc: You are encouraged to develop applications with a single code path to 
	//       respond to device loss. This code path is likely to be similar, if not 
	//       identical, to the code path taken to initialize the device at startup.
	//-----------------------------------------------------------------------------
	HRESULT	restoreDeviceObjects()
	{
		if( !m_pD2DFactory || !m_pDWriteFactory ) return E_FAIL;

		static const WCHAR msc_fontName[] = L"Verdana";
		static const FLOAT msc_fontSize = 50;
		HRESULT hr;

		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_pTextFormat
			);

		if (SUCCEEDED(hr))
		{
			// Center the text horizontally and vertically.
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		// Create a black brush.
		hr = m_pHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&m_pBlackBrush
			);

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: invalidateDeviceObjects()
	// Desc: If the lost device can be restored, the application prepares the 
	//       device by destroying all video-memory resources and any 
	//       swap chains. This is typically accomplished by using the SAFE_RELEASE 
	//       macro.
	//-----------------------------------------------------------------------------
	HRESULT	invalidateDeviceObjects()
	{
		if( !m_pD2DFactory || !m_pDWriteFactory ) return E_FAIL;

		SAFE_RELEASE( m_pBlackBrush );
		SAFE_RELEASE( m_pTextFormat );

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: initialize()
	// Desc: This function will only be called once during the application's 
	//       initialization phase. Therefore, it can't contain any resources that 
	//       need to be restored every time the Direct3D device is lost or the 
	//       window is resized.
	//-----------------------------------------------------------------------------
	HRESULT	initialize()
	{
		HRESULT hr = S_OK;

		m_pD2DFactory = 0;
		m_pWICFactory = 0;
		m_pDWriteFactory = 0;
		m_pHwndRenderTarget = 0;

		// Create D2D factory
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
		if (SUCCEEDED(hr))
		{
			// Create a WIC factory
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory,
				reinterpret_cast<void **>(&m_pWICFactory)
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create DWrite factory
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
				);
		}

		if (SUCCEEDED(hr))
		{
			D2D1_SIZE_U size = D2D1::SizeU( width(), height() );

			// Create a Direct2D render target.
			hr = m_pD2DFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(winId(), size),
				&m_pHwndRenderTarget
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = restoreDeviceObjects();
		}

		return hr;
	}

	//-----------------------------------------------------------------------------
	// Name: uninitialize()
	// Desc: Releases all previously initialized objects
	//-----------------------------------------------------------------------------
	void	uninitialize()
	{
		invalidateDeviceObjects();

		SAFE_RELEASE(m_pHwndRenderTarget);
		SAFE_RELEASE(m_pD2DFactory);
		SAFE_RELEASE(m_pWICFactory);
		SAFE_RELEASE(m_pDWriteFactory);
	}

	//-----------------------------------------------------------------------------
	// Name: clearRenderTarget()
	// Desc: Clear the render target
	//-----------------------------------------------------------------------------
	void	clearRenderTarget( D2D1::ColorF ClearColor )
	{
		m_pHwndRenderTarget->Clear( ClearColor );
	}

	//-----------------------------------------------------------------------------
	// Name: beginDraw()
	// Desc: Begin draw
	//-----------------------------------------------------------------------------
	void	beginDraw()
	{
		m_pHwndRenderTarget->BeginDraw();
	}

	//-----------------------------------------------------------------------------
	// Name: endDraw()
	// Desc: End draw
	//-----------------------------------------------------------------------------
	HRESULT	endDraw()
	{
		HRESULT hr = m_pHwndRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			uninitialize();
			hr = initialize();
		}
		return hr;
	}

	//-----------------------------------------------------------------------------
	// Name: render()
	// Desc: Draws the scene
	//-----------------------------------------------------------------------------
	virtual HRESULT	render()
	{
		if( !m_pHwndRenderTarget ) return E_FAIL;
		if( (m_pHwndRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)) return E_FAIL;

		HRESULT hr = S_OK;

		static const WCHAR sc_helloWorld[] = L"Hello, World!";

		beginDraw();

		m_pHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		clearRenderTarget(D2D1::ColorF(D2D1::ColorF::White));

		m_pHwndRenderTarget->DrawText(
			sc_helloWorld,
			ARRAYSIZE(sc_helloWorld) - 1,
			m_pTextFormat,
			D2D1::RectF(0, 0, width(), height() ),
			m_pBlackBrush
			);

		endDraw();

		return S_OK;
	}

	void	onResize( UINT nWidth, UINT nHeight )
	{
		HRESULT hr = S_OK;

		if( m_pHwndRenderTarget )
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here -- it will be repeated on the next call to
			// EndDraw.
			D2D1_SIZE_U size;
			size.width = nWidth;
			size.height = nHeight;

			m_pHwndRenderTarget->Resize(size);
		}
		render();
	}
private:
	ID2D1Factory*			m_pD2DFactory;

	IWICImagingFactory*		m_pWICFactory;

	IDWriteFactory*			m_pDWriteFactory;

	ID2D1HwndRenderTarget*	m_pHwndRenderTarget;

	ID2D1SolidColorBrush*	m_pBlackBrush;

	IDWriteTextFormat*		m_pTextFormat;

};
