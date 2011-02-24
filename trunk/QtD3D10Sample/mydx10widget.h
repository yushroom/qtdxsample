#pragma once

#define USE_D3D 10
#include "../common/d3dwidget.h"

ALIGN(16) class MyDX10Widget : public DXWidget
{
	//Q_OBJECT

public:
	// A structure for our custom vertex type
	struct SCENE_VERTEX
	{
		D3DXVECTOR2 pos;
	};

	struct VS_CONSTANT_BUFFER
	{
		D3DXMATRIX mWorldViewProj;      //mWorldViewProj will probably be global to all shaders in a project.
		//It's a good idea not to move it around between shaders.
		D3DXVECTOR4 vSomeVectorThatMayBeNeededByASpecificShader;
		float fSomeFloatThatMayBeNeededByASpecificShader;
		float fTime;                    //fTime may also be global to all shaders in a project.
		float fSomeFloatThatMayBeNeededByASpecificShader2;
		float fSomeFloatThatMayBeNeededByASpecificShader3;
	};

	MyDX10Widget( QWidget *parent = 0, Qt::WFlags flags = 0 )
		: DXWidget( parent, flags )
	{
		m_pDevice = 0;
		m_pSwapChain = 0;
		m_pRenderTargetView = 0;
		m_pDepthStencil = 0;
		m_pDepthStencilView = 0;
		m_FeatureLevel = D3D10_FEATURE_LEVEL_9_1;

		m_pVertexLayout = NULL;
		m_pVB10 = NULL;
		m_pIB10 = NULL;
		m_pVS10 = NULL;
		m_pPS10 = NULL;
		m_pConstantBuffer10 = NULL;
		m_pBlendStateNoBlend = NULL;
		m_pRasterizerStateNoCull = NULL;
		m_pLessEqualDepth = NULL;
	}

	virtual ~MyDX10Widget()
	{
		uninitialize();
	}

	//-----------------------------------------------------------------------------
	// Name: initialize()
	// Desc: This function will only be called once during the application's 
	//       initialization phase. Therefore, it can't contain any resources that 
	//       need to be restored every time the Direct3D device is lost or the 
	//       window is resized.
	//-----------------------------------------------------------------------------
	HRESULT initialize()
	{
		HRESULT hr = S_OK;

		static const D3D10_DRIVER_TYPE driverAttempts[] =
		{
			D3D10_DRIVER_TYPE_HARDWARE,
			D3D10_DRIVER_TYPE_WARP,
		};

		static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
		{
			D3D10_FEATURE_LEVEL_10_1,  // Direct3D 10.1 SM 4
			D3D10_FEATURE_LEVEL_10_0,  // Direct3D 10.0 SM 4
			D3D10_FEATURE_LEVEL_9_3,   // Direct3D 9.3  SM 3
			D3D10_FEATURE_LEVEL_9_2,   // Direct3D 9.2  SM 2
			D3D10_FEATURE_LEVEL_9_1,   // Direct3D 9.1  SM 2
		};

		UINT deviceFlags=0;
	#ifdef _DEBUG
		deviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	#endif
	#ifdef USE_D2D
		deviceFlags |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;
	#endif

		for (UINT driver = 0; driver < ARRAYSIZE(driverAttempts); driver++)
		{
			for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
			{
				hr = D3D10CreateDevice1(
					0,
					driverAttempts[driver],
					NULL,
					deviceFlags,
					levelAttempts[level],
					D3D10_1_SDK_VERSION,
					&m_pDevice
					);

				if (SUCCEEDED(hr))
				{
					m_FeatureLevel = levelAttempts[level];
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

			for( UINT i=1; i<=4/*D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT*/; i++ )
			{
				UINT Quality;
				if SUCCEEDED( m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_B8G8R8A8_UNORM, i, &Quality ) )
				{
					if ( Quality > 0 )
					{
						DXGI_SAMPLE_DESC Desc = { i, Quality - 1 };
						m_swapDesc.SampleDesc = Desc;
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

		if (SUCCEEDED(hr))
		{
			hr = restoreDeviceObjects();
		}

		initCamera();

		return hr;
	}

	//-----------------------------------------------------------------------------
	// Name: uninitialize()
	// Desc: Releases all previously initialized objects
	//-----------------------------------------------------------------------------
	void uninitialize()
	{
		invalidateDeviceObjects();

		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pSwapChain);
		SAFE_RELEASE(m_pRenderTargetView);
		SAFE_RELEASE(m_pDepthStencil);
		SAFE_RELEASE(m_pDepthStencilView);
	}

	//-----------------------------------------------------------------------------
	// Name: restoreDeviceObjects()
	// Desc: You are encouraged to develop applications with a single code path to 
	//       respond to device loss. This code path is likely to be similar, if not 
	//       identical, to the code path taken to initialize the device at startup.
	//-----------------------------------------------------------------------------
	HRESULT	restoreDeviceObjects()
	{
		if( !m_pDevice ) return E_FAIL;

		HRESULT hr;

		DWORD VERTS_PER_EDGE = 64;
		DWORD dwNumVertices = VERTS_PER_EDGE * VERTS_PER_EDGE;
		DWORD dwNumIndices = 6 * ( VERTS_PER_EDGE - 1 ) * ( VERTS_PER_EDGE - 1 );

		// Create a constant buffer
		D3D10_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER );
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		V_RETURN( m_pDevice->CreateBuffer( &cbDesc, NULL, &m_pConstantBuffer10 ) );

		// Create an Index Buffer
		D3D10_BUFFER_DESC ibDesc;
		ibDesc.ByteWidth = dwNumIndices * sizeof( WORD );
		ibDesc.Usage = D3D10_USAGE_DEFAULT;
		ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;

		WORD* pIndexData = new WORD[ dwNumIndices ];
		if( !pIndexData )
			return E_OUTOFMEMORY;
		WORD* pIndices = pIndexData;
		for( DWORD y = 1; y < VERTS_PER_EDGE; y++ )
		{
			for( DWORD x = 1; x < VERTS_PER_EDGE; x++ )
			{
				*pIndices++ = ( WORD )( ( y - 1 ) * VERTS_PER_EDGE + ( x - 1 ) );
				*pIndices++ = ( WORD )( ( y - 0 ) * VERTS_PER_EDGE + ( x - 1 ) );
				*pIndices++ = ( WORD )( ( y - 1 ) * VERTS_PER_EDGE + ( x - 0 ) );

				*pIndices++ = ( WORD )( ( y - 1 ) * VERTS_PER_EDGE + ( x - 0 ) );
				*pIndices++ = ( WORD )( ( y - 0 ) * VERTS_PER_EDGE + ( x - 1 ) );
				*pIndices++ = ( WORD )( ( y - 0 ) * VERTS_PER_EDGE + ( x - 0 ) );
			}
		}

		D3D10_SUBRESOURCE_DATA ibInitData;
		ZeroMemory( &ibInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
		ibInitData.pSysMem = pIndexData;
		V_RETURN( m_pDevice->CreateBuffer( &ibDesc, &ibInitData, &m_pIB10 ) );
		SAFE_DELETE_ARRAY( pIndexData );

		// Create a Vertex Buffer
		D3D10_BUFFER_DESC vbDesc;
		vbDesc.ByteWidth = dwNumVertices * sizeof( SCENE_VERTEX );
		vbDesc.Usage = D3D10_USAGE_DEFAULT;
		vbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0;

		SCENE_VERTEX* pVertData = new SCENE_VERTEX[ dwNumVertices ];
		if( !pVertData )
			return E_OUTOFMEMORY;
		SCENE_VERTEX* pVertices = pVertData;
		for( DWORD y = 0; y < VERTS_PER_EDGE; y++ )
		{
			for( DWORD x = 0; x < VERTS_PER_EDGE; x++ )
			{
				( *pVertices++ ).pos = D3DXVECTOR2( ( ( float )x / ( float )( VERTS_PER_EDGE - 1 ) - 0.5f ) * D3DX_PI,
													( ( float )y / ( float )( VERTS_PER_EDGE - 1 ) - 0.5f ) * D3DX_PI );
			}
		}

		D3D10_SUBRESOURCE_DATA vbInitData;
		ZeroMemory( &vbInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
		vbInitData.pSysMem = pVertData;
		V_RETURN( m_pDevice->CreateBuffer( &vbDesc, &vbInitData, &m_pVB10 ) );
		SAFE_DELETE_ARRAY( pVertData );

		// Compile the VS from the file
		ID3D10Blob* pVSBuf = NULL;

		DWORD dwShaderFlags = 0;
		#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3D10_SHADER_DEBUG;
		#endif

		char* vsFunc = 0;
		char* psFunc = 0;

		switch( m_FeatureLevel )
		{
			case D3D10_FEATURE_LEVEL_10_1:
				vsFunc = "vs_4_1";
				psFunc = "ps_4_1";
				break;
			case D3D10_FEATURE_LEVEL_10_0:
				vsFunc = "vs_4_0";
				psFunc = "ps_4_0";
				break;
			case D3D10_FEATURE_LEVEL_9_3:
				vsFunc = "vs_4_0_level_9_3";
				psFunc = "ps_4_0_level_9_3";
				break;
			default:
			case D3D10_FEATURE_LEVEL_9_2: // Shader model 2 fits feature level 9_1
			case D3D10_FEATURE_LEVEL_9_1:
				vsFunc = "vs_4_0_level_9_1";
				psFunc = "ps_4_0_level_9_1";
				break;
		}

		V_RETURN( D3DX10CompileFromFile( L"HLSLwithoutFX10.vsh", NULL, NULL, "Ripple", vsFunc, dwShaderFlags, NULL, NULL, &pVSBuf, NULL,
										 NULL ) );
		V_RETURN( m_pDevice->CreateVertexShader( ( DWORD* )pVSBuf->GetBufferPointer(),
												  pVSBuf->GetBufferSize(), &m_pVS10 ) );

		// Compile the PS from the file
		ID3D10Blob* pPSBuf = NULL;
		V_RETURN( D3DX10CompileFromFile( L"HLSLwithoutFX.psh", NULL, NULL, "main", psFunc, dwShaderFlags, NULL, NULL, &pPSBuf, NULL,
										 NULL ) );
		V_RETURN( m_pDevice->CreatePixelShader( ( DWORD* )pPSBuf->GetBufferPointer(),
												 pPSBuf->GetBufferSize(), &m_pPS10 ) );

		// Create our vertex input layout
		const D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		V_RETURN( m_pDevice->CreateInputLayout( layout, 1, pVSBuf->GetBufferPointer(),
												 pVSBuf->GetBufferSize(), &m_pVertexLayout ) );

		SAFE_RELEASE( pVSBuf );
		SAFE_RELEASE( pPSBuf );

		// Create a blend state to disable alpha blending
		D3D10_BLEND_DESC BlendState;
		ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
		BlendState.BlendEnable[0] = FALSE;
		BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		V_RETURN( m_pDevice->CreateBlendState( &BlendState, &m_pBlendStateNoBlend ) );

		// Create a rasterizer state to disable culling
		D3D10_RASTERIZER_DESC RSDesc;
		RSDesc.FillMode = D3D10_FILL_SOLID;
		RSDesc.CullMode = D3D10_CULL_NONE;
		RSDesc.FrontCounterClockwise = TRUE;
		RSDesc.DepthBias = 0;
		RSDesc.DepthBiasClamp = 0;
		RSDesc.SlopeScaledDepthBias = 0;
		RSDesc.ScissorEnable = FALSE;
		RSDesc.MultisampleEnable = TRUE;
		RSDesc.AntialiasedLineEnable = FALSE;
		V_RETURN( m_pDevice->CreateRasterizerState( &RSDesc, &m_pRasterizerStateNoCull ) );

		// Create a depth stencil state to enable less-equal depth testing
		D3D10_DEPTH_STENCIL_DESC DSDesc;
		ZeroMemory( &DSDesc, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
		DSDesc.DepthEnable = TRUE;
		DSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
		DSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		V_RETURN( m_pDevice->CreateDepthStencilState( &DSDesc, &m_pLessEqualDepth ) );

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
		if( !m_pDevice ) return E_FAIL;

		SAFE_RELEASE( m_pVertexLayout );
		SAFE_RELEASE( m_pVB10 );
		SAFE_RELEASE( m_pIB10 );
		SAFE_RELEASE( m_pVS10 );
		SAFE_RELEASE( m_pPS10 );
		SAFE_RELEASE( m_pConstantBuffer10 );
		SAFE_RELEASE( m_pBlendStateNoBlend );
		SAFE_RELEASE( m_pRasterizerStateNoCull );
		SAFE_RELEASE( m_pLessEqualDepth );

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: render()
	// Desc: Draws the scene
	//-----------------------------------------------------------------------------
	HRESULT	render()
	{
		if( !m_pDevice ) return E_FAIL;
		if( !m_pRenderTargetView || !m_pDepthStencilView ) return E_FAIL;
		if( m_standBy ) 
		{
			return present();
		}

		HRESULT hr = S_OK;

		clearScene( D3DXCOLOR( 0.0f, 0.25f, 0.25f, 0.55f ), 1.0f, 0 );

		D3DXMATRIXA16 mWorldViewProj = D3DXMATRIXA16((float*)&m_viewMatrix) * D3DXMATRIXA16((float*)&m_projMatrix);
		// DX10 spec only guarantees Sincos function from -100 * Pi to 100 * Pi
		float fBoundedTime = (float) m_fTime - (floor( (float) m_fTime / (2.0f * D3DX_PI)) * 2.0f * D3DX_PI);

		DWORD VERTS_PER_EDGE = 64;
		DWORD dwNumIndices = 6 * ( VERTS_PER_EDGE - 1 ) * ( VERTS_PER_EDGE - 1 );

		VS_CONSTANT_BUFFER* pConstData;
		m_pConstantBuffer10->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
		pConstData->mWorldViewProj = mWorldViewProj;
		pConstData->fTime = m_fTime;
		m_pConstantBuffer10->Unmap();

		// Set IA parameters
		UINT strides[1] = { sizeof( SCENE_VERTEX ) };
		UINT offsets[1] = {0};
		ID3D10Buffer* pBuffers[1] = { m_pVB10 };

		m_pDevice->IASetVertexBuffers( 0, 1, pBuffers, strides, offsets );
		m_pDevice->IASetInputLayout( m_pVertexLayout );
		m_pDevice->IASetIndexBuffer( m_pIB10, DXGI_FORMAT_R16_UINT, 0 );
		m_pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		// Bind the constant buffer to the device
		pBuffers[0] = m_pConstantBuffer10;
		m_pDevice->VSSetConstantBuffers( 0, 1, pBuffers );
		m_pDevice->OMSetBlendState( m_pBlendStateNoBlend, 0, 0xffffffff );
		m_pDevice->RSSetState( m_pRasterizerStateNoCull );
		m_pDevice->OMSetDepthStencilState( m_pLessEqualDepth, 0 );
		m_pDevice->VSSetShader( m_pVS10 );
		m_pDevice->GSSetShader( NULL );
		m_pDevice->PSSetShader( m_pPS10 );
		m_pDevice->DrawIndexed( dwNumIndices, 0, 0 );

		hr = present();

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: clearScene()
	// Desc: Clear the render target and depth stencil
	//-----------------------------------------------------------------------------
	void	clearScene( D3DXCOLOR ClearColor, float Z, DWORD Stencil )
	{
		clearRenderTarget( ClearColor );
		clearDepthStencil( Z, Stencil );
	}

	//-----------------------------------------------------------------------------
	// Name: clearRenderTarget()
	// Desc: Clear the render target
	//-----------------------------------------------------------------------------
	void	clearRenderTarget( D3DXCOLOR ClearColor )
	{
		m_pDevice->ClearRenderTargetView( m_pRenderTargetView, (const float*)ClearColor );
	}

	//-----------------------------------------------------------------------------
	// Name: clearScene()
	// Desc: Clear the render target
	//-----------------------------------------------------------------------------
	void	clearDepthStencil( float Z, DWORD Stencil )
	{
		m_pDevice->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH, Z, Stencil );
	}

	//-----------------------------------------------------------------------------
	// Name: present()
	// Desc: Present the backbuffer contents to the display
	//-----------------------------------------------------------------------------
	HRESULT	present()
	{
		HRESULT hr;

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

		return hr;
	}

	void resizeEvent(QResizeEvent *p_event)
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
		
		// Camera --------------------------------------------------------

		setAspect( width() / (float)height() );

		render();
	}

private:
	//! Our rendering device
	ID3D10Device1*				m_pDevice;
	IDXGISwapChain*				m_pSwapChain;
	DXGI_SWAP_CHAIN_DESC		m_swapDesc;
	ID3D10RenderTargetView*		m_pRenderTargetView;
	ID3D10Texture2D*			m_pDepthStencil;
	ID3D10DepthStencilView*		m_pDepthStencilView;
	D3D10_FEATURE_LEVEL1		m_FeatureLevel;

	ID3D10InputLayout*          m_pVertexLayout;
	ID3D10Buffer*               m_pVB10;
	ID3D10Buffer*               m_pIB10;
	ID3D10VertexShader*         m_pVS10;
	ID3D10PixelShader*          m_pPS10;
	ID3D10Buffer*               m_pConstantBuffer10;
	ID3D10BlendState*           m_pBlendStateNoBlend;
	ID3D10RasterizerState*      m_pRasterizerStateNoCull;
	ID3D10DepthStencilState*	m_pLessEqualDepth;

};
