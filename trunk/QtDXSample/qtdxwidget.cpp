#pragma once

#include "qtdxwidget.h"

DXWidget::DXWidget( QWidget *parent, Qt::WFlags flags )
	: QWidget( parent, flags )
{
#if USE_D3D==9
	m_pD3D = 0;
	m_pDevice = 0;
#endif

#if USE_D3D>=10
	m_pDevice = 0;
	m_pSwapChain = 0;
	m_pRenderTargetView = 0;
	m_pDepthStencil = 0;
	m_pDepthStencilView = 0;
#endif

#if USE_D3D==11
	m_pDeviceContext = 0;
#endif

#ifdef USE_D2D
	m_pD2DFactory = 0;
	m_pWICFactory = 0;
	m_pDWriteFactory = 0;
#endif

	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);

	m_timer.setParent( parent );
	m_timer.setInterval( 30 ) ; // in msec 
	m_timer.setSingleShot( false ) ; 
	QObject::connect( &m_timer, SIGNAL( timeout() ), this, SLOT( idle() ) ) ; 

	m_fTime = 0.0f;
	m_standBy = false;
}

DXWidget::~DXWidget()
{
	m_timer.stop();

	uninitialize();
}

//-----------------------------------------------------------------------------
// Name: initialize()
// Desc: This function will only be called once during the application's 
//       initialization phase. Therefore, it can't contain any resources that 
//       need to be restored every time the Direct3D device is lost or the 
//       window is resized.
//-----------------------------------------------------------------------------
HRESULT DXWidget::initialize()
{
	HRESULT hr = S_OK;

#if USE_D3D==9
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION); //Standard

	D3DCAPS9 Caps;
	m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps );

	DWORD BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
	// then switch to SWVP.
	if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
		Caps.VertexShaderVersion < D3DVS_VERSION( 2, 0 ) )
	{
		BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality = 0;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; 

	for( int m=0; m<=(int)D3DMULTISAMPLE_4_SAMPLES; m+=2 )
	{
		DWORD QualityBackBuffer = 0;

		hr = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT,
												 D3DDEVTYPE_HAL,
												 m_d3dpp.BackBufferFormat,
												 m_d3dpp.Windowed,
												 (D3DMULTISAMPLE_TYPE)m,
												 &QualityBackBuffer );
		if( FAILED(hr) ) break;
	
		if( QualityBackBuffer>0 )
		{
			m_d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)m;
			m_d3dpp.MultiSampleQuality = QualityBackBuffer-1;
		}
	}

	// Hardware Device
	hr = m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winId(),
		BehaviorFlags, &m_d3dpp, &m_pDevice );

	if( FAILED(hr) )
	{
		m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_d3dpp.MultiSampleQuality = 0;
		// Reference Rasterizer
		hr = m_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, winId(),
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pDevice );
	}
#endif

#if USE_D3D==10
	static const D3D10_DRIVER_TYPE driverAttempts[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_WARP,
	};

	static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
	{
		D3D10_FEATURE_LEVEL_10_1,
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		// D3D10_FEATURE_LEVEL_9_1,
	};

	for (UINT driver = 0; driver < ARRAYSIZE(driverAttempts); driver++)
	{
		for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
		{
			hr = D3D10CreateDevice1(
				0,
				driverAttempts[driver],
				NULL,
				D3D10_CREATE_DEVICE_BGRA_SUPPORT,
				levelAttempts[level],
				D3D10_1_SDK_VERSION,
				&m_pDevice
				);

			if (SUCCEEDED(hr))
			{
				break;
			}
		}
	}

	IDXGIDevice *pDXGIDevice = NULL;
	IDXGIAdapter *pAdapter = NULL;
	IDXGIFactory *pDXGIFactory = NULL;

    if (SUCCEEDED(hr))
    {
        hr = m_pDevice->QueryInterface(&pDXGIDevice);
    }
    if (SUCCEEDED(hr))
    {
        hr = pDXGIDevice->GetAdapter(&pAdapter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));
    }
    if (SUCCEEDED(hr))
    {
        ::ZeroMemory(&m_swapDesc, sizeof(m_swapDesc));

        m_swapDesc.SampleDesc.Count = 1;		//The Number of Multisamples per Level
        m_swapDesc.SampleDesc.Quality = 0;		//between 0(lowest Quality) and one lesser than m_pDevice->CheckMultisampleQualityLevels

		for( UINT i=1; i<=D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT; i++ )
		{
			UINT Quality;
			if SUCCEEDED( m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_B8G8R8A8_UNORM, i, &Quality ) )
			{
				if ( Quality > 0 )
				{
					DXGI_SAMPLE_DESC Desc = { i, Quality - 1 };
					if ( i <= 4 )
					{
						m_swapDesc.SampleDesc = Desc;
					}
				}
			}
		} 

        m_swapDesc.BufferDesc.Width = width();
        m_swapDesc.BufferDesc.Height = height();
        m_swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        m_swapDesc.BufferDesc.RefreshRate.Numerator = 60;
        m_swapDesc.BufferDesc.RefreshRate.Denominator = 1;
        m_swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        m_swapDesc.BufferCount = 1;
        m_swapDesc.OutputWindow = winId();
        m_swapDesc.Windowed = TRUE;

        hr = pDXGIFactory->CreateSwapChain(m_pDevice, &m_swapDesc, &m_pSwapChain);

		if( FAILED(hr) )
		{
	        m_swapDesc.SampleDesc.Count = 1;
	        hr = pDXGIFactory->CreateSwapChain(m_pDevice, &m_swapDesc, &m_pSwapChain);
		}
    }

	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDXGIFactory);
#endif

#ifdef USE_D2D
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
#endif

	if (SUCCEEDED(hr))
	{
		restoreDeviceObjects();
	}

	return hr;
}

//-----------------------------------------------------------------------------
// Name: uninitialize()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
void DXWidget::uninitialize()
{
	invalidateDeviceObjects();

#ifdef USE_D2D
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pDWriteFactory);
#endif

#if USE_D3D>=11
	SAFE_RELEASE(m_pDeviceContext);
#endif

#if USE_D3D>=10
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pDepthStencilView);
#endif

#if USE_D3D==9
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pD3D);
#endif
}

//-----------------------------------------------------------------------------
// Name: restoreDeviceObjects()
// Desc: You are encouraged to develop applications with a single code path to 
//       respond to device loss. This code path is likely to be similar, if not 
//       identical, to the code path taken to initialize the device at startup.
//-----------------------------------------------------------------------------
HRESULT	DXWidget::restoreDeviceObjects()
{
#if USE_D3D>=9
	if( !m_pDevice ) return E_FAIL;
#endif
#if USE_D3D>=11
	if( !m_pDeviceContext ) return E_FAIL;
#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: invalidateDeviceObjects()
// Desc: If the lost device can be restored, the application prepares the 
//       device by destroying all video-memory resources and any 
//       swap chains. This is typically accomplished by using the SAFE_RELEASE 
//       macro.
//-----------------------------------------------------------------------------
HRESULT DXWidget::invalidateDeviceObjects()
{
#if USE_D3D>=9
	if( !m_pDevice ) return E_FAIL;
#endif
#if USE_D3D>=11
	if( !m_pDeviceContext ) return E_FAIL;
#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
HRESULT	DXWidget::render()
{
#if USE_D3D>=9
	if( !m_pDevice ) return E_FAIL;
#endif
#if USE_D3D>=10
	if( !m_pRenderTargetView || !m_pDepthStencilView ) return E_FAIL;
#endif
#if USE_D3D>=11
	if( !m_pDeviceContext ) return E_FAIL;
#endif

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: clearScene()
// Desc: Clear the render target and depth stencil
//-----------------------------------------------------------------------------
void	DXWidget::clearScene( D3DXCOLOR ClearColor, float Z, DWORD Stencil )
{
#if USE_D3D==9
	m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, (DWORD)ClearColor, Z, Stencil );
#else
	clearRenderTarget( ClearColor );
	clearDepthStencil( Z, Stencil );
#endif
}

//-----------------------------------------------------------------------------
// Name: clearRenderTarget()
// Desc: Clear the render target
//-----------------------------------------------------------------------------
void	DXWidget::clearRenderTarget( D3DXCOLOR ClearColor )
{
#if USE_D3D==9
	m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET,(DWORD)ClearColor, 1.0f, 0 );
#elif USE_D3D==10
	m_pDevice->ClearRenderTargetView( m_pRenderTargetView, (const float*)ClearColor );
#elif USE_D3D==11
	m_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, (const float*)ClearColor );
#endif
}

//-----------------------------------------------------------------------------
// Name: clearScene()
// Desc: Clear the render target
//-----------------------------------------------------------------------------
void	DXWidget::clearDepthStencil( float Z, DWORD Stencil )
{
#if USE_D3D==9
	m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET, 0, Z, Stencil );
#elif USE_D3D==10
	m_pDevice->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH, Z, Stencil );
#elif USE_D3D==11
	m_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH, Z, Stencil );
#endif
}

//-----------------------------------------------------------------------------
// Name: beginScene()
// Desc: Begin the scene
//-----------------------------------------------------------------------------
HRESULT	DXWidget::beginScene()
{
#if USE_D3D==9
	return m_pDevice->BeginScene();
#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: endScene()
// Desc: End the scene
//-----------------------------------------------------------------------------
HRESULT	DXWidget::endScene()
{
#if USE_D3D==9
	return m_pDevice->EndScene();
#endif
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: present()
// Desc: Present the backbuffer contents to the display
//-----------------------------------------------------------------------------
HRESULT	DXWidget::present()
{
	HRESULT hr;

#if USE_D3D==9
	hr = m_pDevice->Present( 0, 0, 0, 0 );

	// The following code refer to "Direct3D (DirectX 9.0) Code Samples Page6: Lost Device Recovery".
	// URL: http://www.codesampler.com/dx9src.htm

	//
	// If Present fails with D3DERR_DEVICELOST the application needs to be 
	// notified so it cleanup resources and reset the device.
	//

	if( D3DERR_DEVICELOST == hr )
	{
		// Yield some CPU time to other processes
		Sleep( 100 ); // 100 milliseconds

		//
		// Test the cooperative level to see if it's okay to render.
		// The application can determine what to do on encountering a lost 
		// device by querying the return value of the TestCooperativeLevel 
		// method.
		//

		if( FAILED( hr = m_pDevice->TestCooperativeLevel() ) )
		{
			// The device has been lost but cannot be reset at this time. 
			// Therefore, rendering is not possible and we'll have to return 
			// and try again at a later time.
			if( hr == D3DERR_DEVICELOST )
				return hr;

			// The device has been lost but it can be reset at this time. 
			if( hr == D3DERR_DEVICENOTRESET )
			{
				//
				// If the device can be restored, the application prepares the 
				// device by destroying all video-memory resources and any 
				// swap chains. 
				//

				invalidateDeviceObjects();

				//
				// Then, the application calls the Reset method.
				//
				// Reset is the only method that has an effect when a device 
				// is lost, and is the only method by which an application can 
				// change the device from a lost to an operational state. 
				// Reset will fail unless the application releases all 
				// resources that are allocated in D3DPOOL_DEFAULT, including 
				// those created by the IDirect3DDevice9::CreateRenderTarget 
				// and IDirect3DDevice9::CreateDepthStencilSurface methods.
				//

				hr = m_pDevice->Reset( &m_d3dpp );

				if( FAILED( hr ) )
					return hr;

				//
				// Finally, a lost device must re-create resources (including  
				// video memory resources) after it has been reset.
				//

				restoreDeviceObjects();
			}
		}
	}
#endif

#if USE_D3D>=10
	hr = m_pSwapChain->Present(0, 0);

	if( hr == DXGI_STATUS_OCCLUDED )
	{
		m_standBy = true;
	}
	else
	{
		m_standBy = false;
	}
	
	if (hr == DXGI_ERROR_DEVICE_RESET ||
		hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		uninitialize();
		hr = initialize();
	}
#endif

	return hr;
}

//-----------------------------------------------------------------------------
// Name: startTimer()
// Desc: Start the m_timer
//-----------------------------------------------------------------------------
void DXWidget::startTimer()
{
	m_timer.start();
}

//-----------------------------------------------------------------------------
// Name: stopTimer()
// Desc: Stop the m_timer
//-----------------------------------------------------------------------------
void DXWidget::stopTimer()
{
	m_timer.stop();
}

void DXWidget::idle()
{
	m_fTime += m_timer.interval()/1000.0f;
	render();
}

void DXWidget::paintEvent(QPaintEvent *paintE)
{
	Q_UNUSED(paintE);
	render();
}

void DXWidget::resizeEvent(QResizeEvent *p_event)
{
	QSize newSize = size();
	if(p_event)
	{
		newSize = p_event->size();
		// if( width()==newSize.width() && height()==newSize.height() ) return;
		QWidget::resizeEvent( p_event );
	}

	UINT nWidth = newSize.width();
	UINT nHeight = newSize.height();

	HRESULT hr = S_OK;

#if USE_D3D>=9
	if( !m_pDevice ) return;
#endif
#if USE_D3D>=11
	if( !m_pDeviceContext ) return;
#endif

#if USE_D3D==10

	ID3D10Resource *pBackBufferResource = NULL;
	ID3D10RenderTargetView *viewList[1] = {NULL};

	m_pDevice->ClearState();
	m_pDevice->OMSetRenderTargets(1, viewList, NULL);

	// Ensure that nobody is holding onto one of the old resources
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilView);

	// Resize render target buffers
	hr = m_pSwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	if (SUCCEEDED(hr))
	{
		D3D10_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.Height = nHeight;
		texDesc.Width = nWidth;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = m_swapDesc.SampleDesc.Count;
		texDesc.SampleDesc.Quality = m_swapDesc.SampleDesc.Quality;
		texDesc.Usage = D3D10_USAGE_DEFAULT;

		SAFE_RELEASE(m_pDepthStencil);
		hr = m_pDevice->CreateTexture2D(&texDesc, NULL, &m_pDepthStencil);
	}

	if (SUCCEEDED(hr))
	{
		// Create the render target view and set it on the device
		hr = m_pSwapChain->GetBuffer(
			0,
			IID_PPV_ARGS(&pBackBufferResource)
			);
	}
	if (SUCCEEDED(hr))
	{
		D3D10_RENDER_TARGET_VIEW_DESC renderDesc;
		renderDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		renderDesc.ViewDimension = (m_swapDesc.SampleDesc.Count>1) ? D3D10_RTV_DIMENSION_TEXTURE2DMS : D3D10_RTV_DIMENSION_TEXTURE2D;
		renderDesc.Texture2D.MipSlice = 0;

		hr = m_pDevice->CreateRenderTargetView(pBackBufferResource, &renderDesc, &m_pRenderTargetView);
	}
	if (SUCCEEDED(hr))
	{
		D3D10_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = (m_swapDesc.SampleDesc.Count>1) ? D3D10_DSV_DIMENSION_TEXTURE2DMS : D3D10_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;

		hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil, &depthViewDesc, &m_pDepthStencilView);
	}
	if (SUCCEEDED(hr))
	{
		viewList[0] = m_pRenderTargetView;
		m_pDevice->OMSetRenderTargets(1, viewList, m_pDepthStencilView);

		// Set a new viewport based on the new dimensions
		D3D10_VIEWPORT viewport;
		viewport.Width = nWidth;
		viewport.Height = nHeight;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		m_pDevice->RSSetViewports(1, &viewport);
	}
	SAFE_RELEASE(pBackBufferResource);
	
#ifdef USE_D2D
	IDXGISurface *pBackBuffer = NULL;

	if (SUCCEEDED(hr))
	{
		// Get a surface in the swap chain
		hr = m_pSwapChain->GetBuffer(
			0,
			IID_PPV_ARGS(&pBackBuffer)
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create the DXGI Surface Render Target.
		FLOAT dpiX;
		FLOAT dpiY;
		m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				dpiX,
				dpiY
				);

		// Create a D2D render target which can draw into the surface in the swap chain
		SAFE_RELEASE(m_pBackBufferRT);
		hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pBackBuffer,
			&props,
			&m_pBackBufferRT
			);
	}

	SAFE_RELEASE(pBackBuffer);
#endif

#endif

#if USE_D3D==9
	m_d3dpp.BackBufferWidth = newSize.width();
	m_d3dpp.BackBufferHeight = newSize.height();

	invalidateDeviceObjects();

	hr = m_pDevice->Reset(&m_d3dpp);

	restoreDeviceObjects();
#endif
}
