#include "stdafx.h"
#include "MiscUtils.h"

void iaSetBuffer( ID3D11Buffer* vb, UINT vbStride, ID3D11Buffer* ib, DXGI_FORMAT ibFormat )
{
	UINT off = 0;
	context->IASetVertexBuffers( 0, 1, &vb, &vbStride, &off );
	context->IASetIndexBuffer( ib, ibFormat, 0 );
}

void iaClearBuffers()
{
	ID3D11Buffer* const buffers[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ] = {};
	UINT zeros[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ] = {};
	context->IASetVertexBuffers( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, buffers, zeros, zeros );
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