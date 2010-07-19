#pragma once

#include "qtdxwidget.h"

class MyDX10Widget : public DXWidget
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

	//-----------------------------------------------------------------------------
	// Name: restoreDeviceObjects()
	// Desc: You are encouraged to develop applications with a single code path to 
	//       respond to device loss. This code path is likely to be similar, if not 
	//       identical, to the code path taken to initialize the device at startup.
	//-----------------------------------------------------------------------------
	HRESULT	restoreDeviceObjects()
	{
		if( FAILED(DXWidget::restoreDeviceObjects()) ) return E_FAIL;

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

		V_RETURN( D3DX10CompileFromFile( L"HLSLwithoutFX10.vsh", NULL, NULL, "Ripple", "vs_4_0", dwShaderFlags, NULL, NULL, &pVSBuf, NULL,
										 NULL ) );
		V_RETURN( m_pDevice->CreateVertexShader( ( DWORD* )pVSBuf->GetBufferPointer(),
												  pVSBuf->GetBufferSize(), &m_pVS10 ) );

		// Compile the PS from the file
		ID3D10Blob* pPSBuf = NULL;
		V_RETURN( D3DX10CompileFromFile( L"HLSLwithoutFX.psh", NULL, NULL, "main", "ps_4_0", dwShaderFlags, NULL, NULL, &pPSBuf, NULL,
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
		DXWidget::invalidateDeviceObjects();

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
		if( FAILED(DXWidget::render()) ) return E_FAIL;
		if( m_standBy ) 
		{
			return present();
		}

		HRESULT hr = S_OK;

		clearScene( D3DXCOLOR( 0.0f, 0.25f, 0.25f, 0.55f ), 1.0f, 0 );

		D3DXMATRIXA16 mWorldViewProj = m_mWorld * m_mView * m_mProj;
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

	ID3D10InputLayout*          m_pVertexLayout;
	ID3D10Buffer*               m_pVB10;
	ID3D10Buffer*               m_pIB10;
	ID3D10VertexShader*         m_pVS10;
	ID3D10PixelShader*          m_pPS10;
	ID3D10Buffer*               m_pConstantBuffer10;
	ID3D10BlendState*           m_pBlendStateNoBlend;
	ID3D10RasterizerState*      m_pRasterizerStateNoCull;
	ID3D10DepthStencilState*	m_pLessEqualDepth;

	D3DXMATRIX					m_mWorld;        // World matrix of model
	D3DXMATRIX					m_mView;         // View matrix 
	D3DXMATRIX					m_mProj;         // Projection matrix

};
