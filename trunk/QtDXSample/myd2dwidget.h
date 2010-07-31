#pragma once

#include "qtdxwidget.h"

class MyD2DWidget : public DXWidget
{
	//Q_OBJECT

public:
	MyD2DWidget( QWidget *parent = 0, Qt::WFlags flags = 0 )
		: DXWidget( parent, flags )
	{
	}

	//-----------------------------------------------------------------------------
	// Name: restoreDeviceObjects()
	// Desc: You are encouraged to develop applications with a single code path to 
	//       respond to device loss. This code path is likely to be similar, if not 
	//       identical, to the code path taken to initialize the device at startup.
	//-----------------------------------------------------------------------------
	HRESULT	restoreDeviceObjects()
	{
		if( FAILED(DXWidget::restoreDeviceObjects()) ) return E_FAIL;

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
	HRESULT invalidateDeviceObjects()
	{
		DXWidget::invalidateDeviceObjects();

		SAFE_RELEASE( m_pBlackBrush );
		SAFE_RELEASE( m_pTextFormat );

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: render()
	// Desc: Draws the scene
	//-----------------------------------------------------------------------------
	virtual HRESULT	render()
	{
		if( FAILED(DXWidget::render()) ) return E_FAIL;

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

	void	resizeEvent( QResizeEvent *p_event )
	{
		DXWidget::resizeEvent( p_event );


	}


private:

	ID2D1SolidColorBrush *m_pBlackBrush;
	IDWriteTextFormat *m_pTextFormat;

};
