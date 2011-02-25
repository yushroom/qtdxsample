#pragma once

#define USE_D3D 9
#include "../common/d3dwidget.h"
#include "../common/logging.h"

class MyDX9Widget : public DXWidget
{
	//Q_OBJECT

public:
	MyDX9Widget( QWidget *parent = 0, Qt::WFlags flags = 0 )
		: DXWidget( parent, flags )
	{
		m_pD3D = 0;
		m_pDevice = 0;
		
		m_pVB = NULL;
		m_pIB = NULL;
		m_pVertexShader = NULL;
		m_pConstantTable = NULL;
		m_pVertexDeclaration = NULL;
	}

	virtual ~MyDX9Widget()
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
		SAFE_RELEASE(m_pD3D);
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

		HRESULT hr = S_OK;

		DWORD VERTS_PER_EDGE = 64;
		DWORD dwNumVertices = VERTS_PER_EDGE * VERTS_PER_EDGE;
		DWORD dwNumIndices = 6 * ( VERTS_PER_EDGE - 1 ) * ( VERTS_PER_EDGE - 1 );

		// Create and initialize index buffer
		WORD* pIndices;
		V_RETURN( m_pDevice->CreateIndexBuffer( dwNumIndices * sizeof( WORD ),
												 0, D3DFMT_INDEX16,
												 D3DPOOL_DEFAULT, &m_pIB, NULL ) );
		V_RETURN( m_pIB->Lock( 0, 0, ( void** )&pIndices, 0 ) );

		DWORD y;
		for( y = 1; y < VERTS_PER_EDGE; y++ )
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
		V_RETURN( m_pIB->Unlock() );

		// Create and initialize vertex buffer
		V_RETURN( m_pDevice->CreateVertexBuffer( dwNumVertices * sizeof( D3DXVECTOR2 ), 0, 0,
												  D3DPOOL_DEFAULT, &m_pVB, NULL ) );

		D3DXVECTOR2* pVertices;
		V_RETURN( m_pVB->Lock( 0, 0, ( void** )&pVertices, 0 ) );
		for( y = 0; y < VERTS_PER_EDGE; y++ )
		{
			for( DWORD x = 0; x < VERTS_PER_EDGE; x++ )
			{
				*pVertices++ = D3DXVECTOR2( ( ( float )x / ( float )( VERTS_PER_EDGE - 1 ) - 0.5f ) * D3DX_PI,
											( ( float )y / ( float )( VERTS_PER_EDGE - 1 ) - 0.5f ) * D3DX_PI );
			}
		}
		V_RETURN( hr = m_pVB->Unlock() );


		LPD3DXBUFFER pCode;
		D3DVERTEXELEMENT9 decl[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			D3DDECL_END()
		};
		V_RETURN( m_pDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration ) );

		DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
		dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
	#endif
#ifdef DEBUG_PS
		dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
		V_RETURN( D3DXCompileShaderFromFile( L"HLSLwithoutFX.vsh", NULL, NULL, "Ripple",
											 "vs_2_0", dwShaderFlags, &pCode,
											 NULL, &m_pConstantTable ) );
		hr = m_pDevice->CreateVertexShader( ( DWORD* )pCode->GetBufferPointer(), &m_pVertexShader );
		pCode->Release();
		if( FAILED( hr ) )
			return hr;

		m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
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
		
		SAFE_RELEASE( m_pVertexShader );
		SAFE_RELEASE( m_pConstantTable );
		SAFE_RELEASE( m_pVertexDeclaration );
		SAFE_RELEASE( m_pIB );
		SAFE_RELEASE( m_pVB );

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: render()
	// Desc: Draws the scene
	//-----------------------------------------------------------------------------
	virtual HRESULT	render()
	{
		if( !m_pDevice ) return E_FAIL;

		HRESULT hr = S_OK;

		clearScene( D3DXCOLOR( 0.0f, 0.25f, 0.25f, 0.55f ), 1.0f, 0 );

		DWORD VERTS_PER_EDGE = 64;
		DWORD dwNumVertices = VERTS_PER_EDGE * VERTS_PER_EDGE;
		DWORD dwNumIndices = 6 * ( VERTS_PER_EDGE - 1 ) * ( VERTS_PER_EDGE - 1 );

		if( SUCCEEDED(beginScene()) )
		{
			D3DXMATRIXA16 mWorldViewProj = D3DXMATRIXA16((float*)&ViewMatrix()) * D3DXMATRIXA16((float*)&ProjMatrix());
			// DX10 spec only guarantees Sincos function from -100 * Pi to 100 * Pi
			float fBoundedTime = (float) m_fTime - (floor( (float) m_fTime / (2.0f * D3DX_PI)) * 2.0f * D3DX_PI);

			m_pConstantTable->SetMatrix( m_pDevice, "mWorldViewProj", &mWorldViewProj );
			m_pConstantTable->SetFloat( m_pDevice, "fTime", fBoundedTime );

			m_pDevice->SetVertexDeclaration( m_pVertexDeclaration );
			m_pDevice->SetVertexShader( m_pVertexShader );
			m_pDevice->SetStreamSource( 0, m_pVB, 0, sizeof( D3DXVECTOR2 ) );
			m_pDevice->SetIndices( m_pIB );
			m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, dwNumVertices, 0, dwNumIndices / 3 );

			endScene();
		}

		hr = present();
		m_lastRendered = m_fTime;

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: clearScene()
	// Desc: Clear the render target and depth stencil
	//-----------------------------------------------------------------------------
	void	clearScene( D3DXCOLOR ClearColor, float Z, DWORD Stencil )
	{
		m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, (DWORD)ClearColor, Z, Stencil );
	}

	//-----------------------------------------------------------------------------
	// Name: clearRenderTarget()
	// Desc: Clear the render target
	//-----------------------------------------------------------------------------
	void	clearRenderTarget( D3DXCOLOR ClearColor )
	{
		m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET,(DWORD)ClearColor, 1.0f, 0 );
	}

	//-----------------------------------------------------------------------------
	// Name: clearScene()
	// Desc: Clear the render target
	//-----------------------------------------------------------------------------
	void	clearDepthStencil( float Z, DWORD Stencil )
	{
		m_pDevice->Clear( 0, 0, D3DCLEAR_TARGET, 0, Z, Stencil );
	}

	//-----------------------------------------------------------------------------
	// Name: beginScene()
	// Desc: Begin the scene
	//-----------------------------------------------------------------------------
	HRESULT	beginScene()
	{
		return m_pDevice->BeginScene();
	}

	//-----------------------------------------------------------------------------
	// Name: endScene()
	// Desc: End the scene
	//-----------------------------------------------------------------------------
	HRESULT	endScene()
	{
		return m_pDevice->EndScene();
	}

	//-----------------------------------------------------------------------------
	// Name: present()
	// Desc: Present the backbuffer contents to the display
	//-----------------------------------------------------------------------------
	HRESULT	present()
	{
		HRESULT hr;

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

		return hr;
	}

	virtual void	paintEvent(QPaintEvent *e)
	{
		Q_UNUSED(e);
		render();
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

		if( !m_pDevice ) return;

		m_d3dpp.BackBufferWidth = newSize.width();
		m_d3dpp.BackBufferHeight = newSize.height();

		invalidateDeviceObjects();

		hr = m_pDevice->Reset(&m_d3dpp);

		restoreDeviceObjects();
		
		// Camera ---------------------------------------------------

		setAspect( width() / (float)height() );

		render();
	}

private:
	//! Used to create the D3DDevice
	IDirect3D9*				m_pD3D;

	//! Our rendering device
	IDirect3DDevice9*		m_pDevice;

	//! D3D Device Parameterss
	D3DPRESENT_PARAMETERS	m_d3dpp;

	LPDIRECT3DVERTEXBUFFER9         m_pVB;
	LPDIRECT3DINDEXBUFFER9          m_pIB;
	LPDIRECT3DVERTEXSHADER9         m_pVertexShader;
	LPD3DXCONSTANTTABLE             m_pConstantTable;
	LPDIRECT3DVERTEXDECLARATION9    m_pVertexDeclaration;

};
