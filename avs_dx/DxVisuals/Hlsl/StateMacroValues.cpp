#include "stdafx.h"
#include "StateMacroValues.h"

using namespace Hlsl;

void StateMacroValues::add( const CStringA &key, const CStringA &value )
{
	setMacro( m_map, key, value );
}

void StateMacroValues::add( const CStringA &key, int value )
{
	CStringA str;
	str.Format( "%i", value );
	add( key, str );
}

void StateMacroValues::uintConstants( const CStringA &key, size_t count, const int* arr )
{
	CStringA value;
	for( size_t i = 0; i < count; i++ )
	{
		value.AppendFormat( "0x%08X", arr[ i ] );
		if( i + 1 < count )
			value += ", ";
	}
	add( key, value );
}

CStringA StateMacroValues::expand( CStringA hlsl ) const
{
	for( const auto& m : m_map )
		hlsl.Replace( m.first, m.second );
	return hlsl;
}