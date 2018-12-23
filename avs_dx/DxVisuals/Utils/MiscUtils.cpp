#include "stdafx.h"
#include "MiscUtils.h"
#include "../Resources/staticResources.h"

void iaSetBuffer( ID3D11Buffer* vb, UINT vbStride, ID3D11Buffer* ib, DXGI_FORMAT ibFormat )
{
	UINT off = 0;
	context->IASetVertexBuffers( 0, 1, &vb, &vbStride, &off );
	context->IASetIndexBuffer( ib, ibFormat, 0 );
}

void iaClearBuffer()
{
	ID3D11Buffer* const buffer = nullptr;
	const UINT zero = 0;
	context->IASetVertexBuffers( 0, 1, &buffer, &zero, &zero );
	context->IASetInputLayout( nullptr );
}

LockExternCs::LockExternCs( CRITICAL_SECTION& cs ) : m_cs( cs )
{
	EnterCriticalSection( &m_cs );
}
LockExternCs::~LockExternCs()
{
	LeaveCriticalSection( &m_cs );
}

UnlockExternCs::UnlockExternCs( CRITICAL_SECTION& cs ) : m_cs( cs )
{
	LeaveCriticalSection( &m_cs );
}
UnlockExternCs::~UnlockExternCs()
{
	EnterCriticalSection( &m_cs );
}

void setMacro( std::vector<std::pair<CStringA, CStringA>> &macros, const CStringA& key, const CStringA& value )
{
	for( auto& p : macros )
	{
		if( p.first == key )
		{
			logWarning( "Redefining HLSL macro %s", key.operator const char*( ) );
			p.second = value;
			return;
		}
	}
	macros.emplace_back( std::make_pair( key, value ) );
}

void omBlend()
{
	context->OMSetBlendState( StaticResources::blendPremultipliedAlpha, nullptr, 0xffffffff );
}

void omDontBlend()
{
	context->OMSetBlendState( nullptr, nullptr, 0xffffffff );
}

void drawFullscreenTriangle( bool bindShaders )
{
	if( bindShaders )
	{
		bindShader<eStage::Vertex>( StaticResources::fullScreenTriangle );
		bindShader<eStage::Geometry>( nullptr );
	}
	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->Draw( 3, 0 );
}