#include "stdafx.h"
#include "MiscUtils.h"

void iaClearBuffers()
{
	ID3D11Buffer* const buffers[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ] = {};
	UINT zeros[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ] = {};
	context->IASetVertexBuffers( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, buffers, zeros, zeros );
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