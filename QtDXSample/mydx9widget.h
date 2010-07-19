#pragma once

#include "qtdxwidget.h"

class MyDX9Widget : public DXWidget
{
	//Q_OBJECT

public:
	MyDX9Widget( QWidget *parent = 0, Qt::WFlags flags = 0 )
		: DXWidget( parent, flags )
	{
		m_pVB = NULL;
		m_pIB = NULL;
		m_pVertexShader = NULL;
		m_pConstantTable = NULL;
		m_pVertexDeclaration = NULL;
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
		DXWidget::invalidateDeviceObjects();
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
		if( FAILED(DXWidget::render()) ) return E_FAIL;

		HRESULT hr = S_OK;

		clearScene( D3DXCOLOR( 0.0f, 0.25f, 0.25f, 0.55f ), 1.0f, 0 );

		DWORD VERTS_PER_EDGE = 64;
		DWORD dwNumVertices = VERTS_PER_EDGE * VERTS_PER_EDGE;
		DWORD dwNumIndices = 6 * ( VERTS_PER_EDGE - 1 ) * ( VERTS_PER_EDGE - 1 );

		if( SUCCEEDED(beginScene()) )
		{
			D3DXMATRIXA16 mWorldViewProj = m_mWorld * m_mView * m_mProj;
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

		return S_OK;
	}

	void	resizeEvent( QResizeEvent *p_event )
	{
		DXWidget::resizeEvent( p_event );

		float fAspect = width() / (float)height();
		float fFOV = D3DX_PI / 4;
		float fNearPlane = 0.1f;
		float fFarPlane = 5000.0f;
		D3DXMatrixPerspectiveFovLH( &m_mProj, fFOV, fAspect, fNearPlane, fFarPlane );

		D3DXQUATERNION m_qNow;              // Composite quaternion for current drag
		D3DXMATRIXA16 m_mRotation;         // Matrix for arc ball's orientation

		m_qNow = D3DXQUATERNION( -0.275f, 0.3f, 0.0f, 0.7f );
		D3DXMatrixRotationQuaternion( &m_mRotation, &m_qNow );

		D3DXMATRIX mCameraRot;
		D3DXMatrixInverse( &mCameraRot, NULL, &m_mRotation );

		// Transform vectors based on camera's rotation matrix
		D3DXVECTOR3 vWorldUp, vWorldAhead;
		D3DXVECTOR3 vLocalUp = D3DXVECTOR3( 0, 1, 0 );
		D3DXVECTOR3 vLocalAhead = D3DXVECTOR3( 0, 0, 1 );
		D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
		D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

		float m_fRadius;              // Distance from the camera to model 
		m_fRadius = 5.0f;
		
		D3DXVECTOR3 m_vLookAt( 0, 0, 1 );
		D3DXVECTOR3 m_vModelCenter( 0, 0, 0 );

		// Update the eye point based on a radius away from the lookAt position
		D3DXVECTOR3 m_vEye = m_vLookAt - vWorldAhead * m_fRadius;

		// Update the view matrix
		D3DXMatrixLookAtLH( &m_mView, &m_vEye, &m_vLookAt, &vWorldUp );

		D3DXMATRIX m_mModelRot;
		D3DXMatrixIdentity( &m_mModelRot );

		// Since we're accumulating delta rotations, we need to orthonormalize 
		// the matrix to prevent eventual matrix skew
		D3DXVECTOR3* pXBasis = ( D3DXVECTOR3* )&m_mModelRot._11;
		D3DXVECTOR3* pYBasis = ( D3DXVECTOR3* )&m_mModelRot._21;
		D3DXVECTOR3* pZBasis = ( D3DXVECTOR3* )&m_mModelRot._31;
		D3DXVec3Normalize( pXBasis, pXBasis );
		D3DXVec3Cross( pYBasis, pZBasis, pXBasis );
		D3DXVec3Normalize( pYBasis, pYBasis );
		D3DXVec3Cross( pZBasis, pXBasis, pYBasis );

		// Translate the rotation matrix to the same position as the lookAt position
		m_mModelRot._41 = m_vLookAt.x;
		m_mModelRot._42 = m_vLookAt.y;
		m_mModelRot._43 = m_vLookAt.z;

		// Translate world matrix so its at the center of the model
		D3DXMATRIX mTrans;
		D3DXMatrixTranslation( &mTrans, -m_vModelCenter.x, -m_vModelCenter.y, -m_vModelCenter.z );
		m_mWorld = mTrans * m_mModelRot;

	}


private:

	LPDIRECT3DVERTEXBUFFER9         m_pVB;
	LPDIRECT3DINDEXBUFFER9          m_pIB;
	LPDIRECT3DVERTEXSHADER9         m_pVertexShader;
	LPD3DXCONSTANTTABLE             m_pConstantTable;
	LPDIRECT3DVERTEXDECLARATION9    m_pVertexDeclaration;

	D3DXMATRIX						m_mWorld;        // World matrix of model
	D3DXMATRIX						m_mView;         // View matrix 
	D3DXMATRIX						m_mProj;         // Projection matrix

};
