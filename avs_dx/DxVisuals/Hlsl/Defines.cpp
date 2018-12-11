#include "stdafx.h"
#include "Defines.h"

using namespace Hlsl;

void Defines::set( const CStringA& key, const CStringA& value )
{
	for( auto& p : m_map )
	{
		if( p.first == key )
		{
			logWarning( "Redefining HLSL macro %s", key.operator const char*() );
			p.second = value;
			return;
		}
	}
	m_map.emplace_back( std::make_pair( key, value ) );
}

void Defines::getData( const Defines* pDefines, std::vector<D3D_SHADER_MACRO>& macros )
{
	macros.clear();
	macros.emplace_back( D3D_SHADER_MACRO{ "AVS_SHADER", "" } );

	if( nullptr != pDefines )
	{
		for( auto& p : pDefines->m_map )
			macros.emplace_back( D3D_SHADER_MACRO{ p.first, p.second } );
	}
	macros.emplace_back( D3D_SHADER_MACRO{ nullptr, nullptr } );
}