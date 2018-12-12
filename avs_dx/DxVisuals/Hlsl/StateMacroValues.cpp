#include "stdafx.h"
#include "StateMacroValues.h"

using namespace Hlsl;

StateMacroValues::StateMacroValues( int stateBufferOffset )
{
	m_map.reserve( 4 );
	add( "STATE_OFFSET", stateBufferOffset );
}

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

CStringA StateMacroValues::expand( CStringA hlsl ) const
{
	for( const auto& m : m_map )
		hlsl.Replace( m.first, m.second );
	return hlsl;
}