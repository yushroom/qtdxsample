#pragma once

#include "common.h"

#include <QWidget>
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
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pD3D);
	}

    HRESULT InitD3D()
	{
		HRESULT hr = S_OK;

		pD3D = Direct3DCreate9(D3D_SDK_VERSION); //Standard
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16; 

		V_RETURN( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, winId(), // DXREF Hardware Vertex GPU
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &pDevice ) )

		attach();

		return S_OK;
	}

	HRESULT	attach()
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

	HRESULT	render()
	{
		if(pDevice == 0) return E_FAIL;

		pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		if(FAILED(attach())) return E_FAIL;

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
		pDevice->Present( 0, 0, 0, 0 );

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

	//! Buffer to hold Vertices
	LPDIRECT3DVERTEXBUFFER9 pVB;
};
