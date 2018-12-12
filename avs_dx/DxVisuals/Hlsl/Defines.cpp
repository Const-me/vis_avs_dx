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

std::vector<D3D_SHADER_MACRO> Defines::data() const
{
	std::vector<D3D_SHADER_MACRO> macros;
	macros.reserve( m_map.size() + 2 );

	macros.emplace_back( D3D_SHADER_MACRO{ "AVS_SHADER", "" } );

	for( auto& p : m_map )
		macros.emplace_back( D3D_SHADER_MACRO{ p.first, p.second } );

	macros.emplace_back( D3D_SHADER_MACRO{ nullptr, nullptr } );

	return std::move( macros );
}

void Defines::set( const CStringA &key, uint32_t value )
{
	CStringA str;
	str.Format( "0x%X", value );
	set( key, str );
}

void Defines::set( const CStringA &key, int value )
{
	CStringA str;
	str.Format( "%i", value );
	set( key, str );
}

void Defines::set( const CStringA &key, float value )
{
	CStringA str;
	str.Format( "%g", value );
	set( key, str );
}

void Defines::set( const CStringA &key, const std::vector<int>& vec )
{
	CStringA str;
	for( int i : vec )
	{
		if( str.GetLength() > 0)
			str += ", ";
		str.AppendFormat( "0x%X", i );
	}
	set( key, str );
}

CStringA Defines::expand( CStringA hlsl ) const
{
	for( const auto& m : m_map )
		hlsl.Replace( m.first, m.second );
	return hlsl;
}