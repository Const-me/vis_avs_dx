#include "stdafx.h"
#include "Defines.h"

using namespace Hlsl;

void Defines::set( const CStringA& name, const CStringA& value )
{
	for( auto& p : m_map )
	{
		if( p.first == name )
		{
			logWarning( "Redefining HLSL macro %s", name.operator const char*() );
			p.second = value;
			return;
		}
	}
	m_map.emplace_back( std::make_pair( name, value ) );
}

void Defines::reset( const CStringA& name, const char* value )
{
	for( auto& p : m_map )
	{
		if( p.first == name )
		{
			p.second = value;
			return;
		}
	}
	assert( false );
	set( name, value );
}

void Defines::set( const CStringA& name, const char* value )
{
	set( name, CStringA{ value } );
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

void Defines::set( const CStringA &key, bool value )
{
	const CStringA s = ( value ) ? "1" : "0";
	set( key, s );
}

void Defines::set( const CStringA &key, const vector<int>& vec )
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

void Defines::set( const CStringA &key, const float2& value )
{
	CStringA str;
	str.Format( "float2( %g, %g )", value.x, value.y );
	set( key, str );
}

void Defines::set( const CStringA &key, const float3& value )
{
	CStringA str;
	str.Format( "float3( %g, %g, %g )", value.x, value.y, value.z );
	set( key, str );
}

void Defines::set( const CStringA &key, const float4& value )
{
	CStringA str;
	str.Format( "float4( %g, %g, %g, %g )", value.x, value.y, value.z, value.w );
	set( key, str );
}

void Defines::set( const CStringA &key, const uint2& value )
{
	CStringA str;
	str.Format( "uint2( %i, %i )", value.cx, value.cy );
	set( key, str );
}

void Defines::setBinding( const CStringA &key, char type, UINT slot )
{
	if( slot >= 1024 )
		__debugbreak();

	CStringA value;
	value.Format( "%c%i", type, slot );
	set( key, value );
}

vector<D3D_SHADER_MACRO> Defines::data() const
{
	vector<D3D_SHADER_MACRO> macros;
	macros.reserve( m_map.size() + 2 );

	macros.emplace_back( D3D_SHADER_MACRO{ "AVS_SHADER", "1" } );

	for( auto& p : m_map )
		macros.emplace_back( D3D_SHADER_MACRO{ p.first, p.second } );

	macros.emplace_back( D3D_SHADER_MACRO{ nullptr, nullptr } );

	return eastl::move( macros );
}