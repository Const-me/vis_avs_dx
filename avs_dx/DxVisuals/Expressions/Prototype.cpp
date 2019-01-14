#include "stdafx.h"
#include "Prototype.h"
#include "utils.hpp"

using namespace Expressions;

HRESULT Prototype::addState( const CStringA& name, uint32_t def, bool dontStore )
{
	CStringA str;
	str.Format( "%i", def );
	return addState( name, eVarType::u32, str, dontStore );
}

HRESULT Prototype::addState( const CStringA& name, float def )
{
	CStringA str;
	str.Format( "%g", def );
	return addState( name, eVarType::f32, str );
}

HRESULT Prototype::addState( const CStringA& name, eVarType vt, const CStringA& initVal, bool dontStore )
{
	for( const auto& s : m_fixedState )
		if( s.name == name )
		{
			__debugbreak();
			return E_INVALIDARG;
		}

	m_fixedState.emplace_back( FixedStateVar{ vt, name, initVal, m_size, dontStore } );;
	m_size += variableSize( vt );
	return S_OK;
}

HRESULT Prototype::addVariable( eVarLocation loc, eVarType vt, const CStringA& name )
{
	for( const auto& s : m_vars )
		if( s.name == name )
		{
			__debugbreak();
			return E_INVALIDARG;
		}
	m_vars.emplace_back( VariableDecl{ loc, vt, name } );
	return S_OK;
}

HRESULT Prototype::addBeatConstant( const CStringA& name )
{
	if( m_beatMacro.GetLength() > 0 )
	{
		assert( false );
		return E_INVALIDARG;
	}
	CHECK( addVariable( eVarLocation::macro, eVarType::u32, name ) );
	m_beatMacro = name;
	return S_OK;
}

CStringA Prototype::initState() const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
		hlsl.AppendFormat( "		%s %s = %s;\r\n", hlslName( s.vt ), cstr( s.name ), cstr( s.initVal ) );
	for( const auto& a : m_indirectArgs )
		hlsl.AppendFormat( "		uint4 %s = uint4( %i, %i, %i, %i );\r\n", cstr( a.name ), a.init[ 0 ], a.init[ 1 ], a.init[ 2 ], a.init[ 3 ] );
	return hlsl;
}

CStringA Prototype::stateLoad( int offset ) const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
	{
		const CStringA load = Expressions::stateLoad( s.vt, s.offset + offset );
		hlsl.AppendFormat( "		%s %s = %s;\r\n", hlslName( s.vt ), cstr( s.name ), cstr( load ) );
	}
	for( const auto& a : m_indirectArgs )
	{
		hlsl.AppendFormat( "		uint4 %s = effectStates.Load4( STATE_OFFSET + %i );\r\n", cstr( a.name ), ( a.offset + offset ) * 4 );
	}
	return hlsl;
}

CStringA Prototype::stateStore( int offset ) const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
	{
		if( s.dontStore )
			continue;
		const CStringA store = Expressions::stateStore( s.vt, s.offset + offset, s.name );
		hlsl.AppendFormat( "		%s;\r\n", cstr( store ) );
	}
	for( const auto& a : m_indirectArgs )
	{
		hlsl.AppendFormat( "		%s;\r\n", cstr( a.update ) );
		hlsl.AppendFormat( "		effectStates.Store4( STATE_OFFSET + %i, %s );\r\n", ( a.offset + offset ) * 4, cstr( a.name ) );
	}
	return hlsl;
}

HRESULT Prototype::addIndirectDrawArgs( const CStringA& name, const uint4& init, const char* update )
{
	for( const auto& s : m_indirectArgs )
		if( s.name == name )
		{
			__debugbreak();
			return E_INVALIDARG;
		}
	m_indirectArgs.emplace_back( IndirectDrawArgs{ name, init, update, m_size } );
	m_size += 4;
	return S_OK;
}