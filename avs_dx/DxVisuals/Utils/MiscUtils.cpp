#include "stdafx.h"
#include "MiscUtils.h"
#include "../Resources/staticResources.h"

void iaSetVertexBuffer( ID3D11Buffer* vb, UINT vbStride )
{
	UINT off = 0;
	context->IASetVertexBuffers( 0, 1, &vb, &vbStride, &off );
}

void iaSetBuffer( ID3D11Buffer* vb, UINT vbStride, ID3D11Buffer* ib, DXGI_FORMAT ibFormat )
{
	iaSetVertexBuffer( vb, vbStride );
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

void omBlend( eBlend mode )
{
	switch( mode )
	{
	case eBlend::None:
		context->OMSetBlendState( nullptr, nullptr, 0xffffffff );
		break;
	case eBlend::Premultiplied:
		context->OMSetBlendState( StaticResources::blendPremultipliedAlpha, nullptr, 0xffffffff );
		break;
	case eBlend::Add:
		context->OMSetBlendState( StaticResources::blendAdditive, nullptr, 0xffffffff );
		break;
	}
}

void omCustomBlend( float f )
{
	const float BlendFactor[ 4 ] = { f, f, f, f };
	context->OMSetBlendState( StaticResources::blendCustom, BlendFactor, 0xffffffff );
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

class CoInit
{
	bool started = false;

public:
	HRESULT startup()
	{
		if( started )
			return S_FALSE;
		CHECK( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) );
		started = true;
		return S_OK;
	}

	HRESULT shutdown()
	{
		if( started )
		{
			logShutdown( "CoUninitialize" );
			CoUninitialize();
			started = false;
			return S_OK;
		}
		return S_FALSE;
	}
};

thread_local CoInit tl_com;

HRESULT coInit()
{
	return tl_com.startup();
}

HRESULT comUninitialize()
{
	return tl_com.shutdown();
}