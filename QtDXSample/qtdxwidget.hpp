#pragma once

#include "common.h"

#include <QWidget.h>
#include <QtCore/QTimer>

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

class DXWidget : public QWidget
{
    Q_OBJECT

public:
	// A structure for our custom vertex type
	struct CUSTOMVERTEX
	{
		D3DXVECTOR4 pos;	// The transformed position for the vertex
		DWORD color;        // The vertex color
	};

    DXWidget(QWidget *parent = 0, Qt::WFlags flags = 0)
		: QWidget(parent, flags)
	{
		pD3D = NULL;
		pDevice = NULL;
		pVB = NULL;

        setAttribute(Qt::WA_PaintOnScreen);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_OpaquePaintEvent);

		timer.setParent( parent );
		timer.setInterval( 30 ) ; // in msec 
		timer.setSingleShot( false ) ; 
		QObject::connect( &timer, SIGNAL( timeout() ), this, SLOT( idle() ) ) ; 

		elapsedTime = 0.0f;
	}

    ~DXWidget()
	{
		timer.stop();

		uninitialize();
	}

    HRESULT initialize()
	{
		HRESULT hr = S_OK;

		pD3D = Direct3DCreate9(D3D_SDK_VERSION); //Standard

		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16; 

		V_RETURN( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winId(), // DXREF Hardware Vertex GPU
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &pDevice ) )

		restoreDeviceObjects();

		return S_OK;
	}

    void uninitialize()
	{
		invalidateDeviceObjects();
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pD3D);
	}

	//-----------------------------------------------------------------------------
	// Name: restoreDeviceObjects()
	// Desc: You are encouraged to develop applications with a single code path to 
	//       respond to device loss. This code path is likely to be similar, if not 
	//       identical, to the code path taken to initialize the device at startup.
	//-----------------------------------------------------------------------------
	HRESULT	restoreDeviceObjects()
	{
		SAFE_RELEASE(pVB);

		HRESULT hr = S_OK;

		// Initialize three Vertex Colors
		D3DXCOLOR	vcolor[3];
		for(int i=0;i<3;i++)
		{
			vcolor[i].r = cos( M_PI * (elapsedTime + i+0) / 1.5f ) * 0.5f + 0.5f;
			vcolor[i].g = cos( M_PI * (elapsedTime + i+1) / 1.5f ) * 0.5f + 0.5f;
			vcolor[i].b = cos( M_PI * (elapsedTime + i+2) / 1.5f ) * 0.5f + 0.5f;
			vcolor[i].a = 1;
		}

		// Initialize three Vertices for rendering a triangle
		CUSTOMVERTEX Vertices[] =
		{
			{ D3DXVECTOR4( 150.0f,  50.0f, 0.5f, 1.0f ), (DWORD)vcolor[0], }, // x, y, z, rhw, color
			{ D3DXVECTOR4( 250.0f, 250.0f, 0.5f, 1.0f ), (DWORD)vcolor[1], },
			{ D3DXVECTOR4(  50.0f, 250.0f, 0.5f, 1.0f ), (DWORD)vcolor[2], },
		};
		
		// Create the vertex buffer. Here we are allocating enough memory
		// (from the default pool) to hold all our 3 custom Vertices. We also
		// specify the FVF, so the vertex buffer knows what data it contains.
		V_RETURN( pDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
													  0, D3DFVF_CUSTOMVERTEX,
													  D3DPOOL_DEFAULT, &pVB, NULL ) )

		// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
		// gain access to the Vertices. This mechanism is required becuase vertex
		// buffers may be in device memory.
		VOID* pVertices;
		V_RETURN( pVB->Lock( 0, sizeof( Vertices ), ( void** )&pVertices, 0 ) )
		memcpy( pVertices, Vertices, sizeof( Vertices ) );
		pVB->Unlock();

		return S_OK;
	}

	//-----------------------------------------------------------------------------
	// Name: invalidateDeviceObjects()
	// Desc: If the lost device can be restored, the application prepares the 
	//       device by destroying all video-memory resources and any 
	//       swap chains. This is typically accomplished by using the SAFE_RELEASE 
	//       macro.
	//-----------------------------------------------------------------------------
	HRESULT invalidateDeviceObjects( void )
	{
		SAFE_RELEASE(pVB);
		return S_OK;
	}

	HRESULT	render()
	{
		if(pDevice == 0) return E_FAIL;

		HRESULT hr = S_OK;
		pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		if(FAILED(restoreDeviceObjects())) return E_FAIL;

		if(SUCCEEDED(pDevice->BeginScene()))
		{
			// Draw the triangles in the vertex buffer. This is broken into a few
			// steps. We are passing the Vertices down a "stream", so first we need
			// to specify the source of that stream, which is our vertex buffer. Then
			// we need to let D3D know what vertex shader to use. Full, custom vertex
			// shaders are an advanced topic, but in most cases the vertex shader is
			// just the FVF, so that D3D knows what type of Vertices we are dealing
			// with. Finally, we call DrawPrimitive() which does the actual rendering
			// of our geometry (in this case, just one triangle).
			pDevice->SetStreamSource( 0, pVB, 0, sizeof( CUSTOMVERTEX ) );
			pDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

			pDevice->EndScene();
		}

		hr = pDevice->Present( 0, 0, 0, 0 );

		// The following code refer to "Direct3D (DirectX 9.0) Code Samples Page6: Lost Device Recovery".
		// URL: http://www.codesampler.com/dx9src.htm

		//
		// If Present fails with D3DERR_DEVICELOST the application needs to be 
		// notified so it cleanup resources and reset the device.
		//

		if( D3DERR_DEVICENOTRESET == hr )
		{
			// Yield some CPU time to other processes
			Sleep( 100 ); // 100 milliseconds

			//
			// Test the cooperative level to see if it's okay to render.
			// The application can determine what to do on encountering a lost 
			// device by querying the return value of the TestCooperativeLevel 
			// method.
			//

			if( FAILED( hr = pDevice->TestCooperativeLevel() ) )
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

					hr = pDevice->Reset( &d3dpp );

					if( FAILED(hr ) )
						return hr;

					//
					// Finally, a lost device must re-create resources (including  
					// video memory resources) after it has been reset.
					//

					restoreDeviceObjects();
				}
			}
		}

		return S_OK;
	}

	void startTimer()
	{
		timer.start();
	}

	void stopTimer()
	{
		timer.stop();
	}

public slots:
	void idle()
	{
		elapsedTime += timer.interval()/1000.0f;
		render();
	}

private:
	QPaintEngine *paintEngine() const { return 0; } 

    void paintEvent(QPaintEvent *paintE)
	{
		Q_UNUSED(paintE);
		render();
	}

	//! Timer
	QTimer	timer;

	//! Elapsed Time
	float	elapsedTime;

	//! Used to create the D3DDevice
	LPDIRECT3D9         pD3D; 

	//! Our rendering device
	LPDIRECT3DDEVICE9   pDevice;

	//! D3D Device Parameters
	D3DPRESENT_PARAMETERS d3dpp;

	//! Buffer to hold Vertices
	LPDIRECT3DVERTEXBUFFER9 pVB;
};
