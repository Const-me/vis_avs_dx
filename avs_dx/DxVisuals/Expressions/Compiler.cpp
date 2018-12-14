#include "stdafx.h"
#include "Compiler.h"
#include "parse.h"
#include "utils.hpp"

using namespace Expressions;

Compiler::Compiler( const char* effectName, const Prototype& effectPrototype ) :
	name( effectName ),
	proto( effectPrototype )
{ }

HRESULT Compiler::update( RString effect_exp[ 4 ] )
{
	int changedMask = 0;
	CStringA ee;
	for( int i = 0; i < 4; i++ )
	{
		ee = effect_exp[ i ].get();
		if( ee == m_expressions[ i ] )
			continue;
		m_expressions[ i ] = ee;
		changedMask |= ( 1 << i );
	}
	if( 0 == changedMask )
		return S_FALSE;

	m_globalsCode.RemoveAll();
	m_vars.clear();
	m_hlslState = "";
	m_hlslFragment = "";
	m_vars.clear();
	m_stateSize = proto.fixedStateSize();
	if( m_expressions[ 0 ].GetLength() <= 0 && m_expressions[ 1 ].GetLength() <= 0 && m_expressions[ 2 ].GetLength() <= 0 && m_expressions[ 3 ].GetLength() <= 0 )
	{
		// All expressions are empty
		return S_OK;
	}

	std::array<Assignments, 4> parsed;
	for( int i = 0; i < 4; i++ )
		CHECK( parseAssignments( m_expressions[ i ], parsed[ i ] ) );

	CHECK( allocVariables( parsed ) );

	return E_NOTIMPL;
}

namespace
{
	eVarType guessVarType( const CStringA& rhs )
	{
		const CStringA fi = getFirstId( rhs );
		if( fi == "gettime" )
			return eVarType::u32;
		return eVarType::f32;
	}
}

HRESULT Compiler::allocVariables( const std::array<Assignments, 4>& parsed )
{
	struct sVarFlags
	{
		bool builtin = false;
		eVarType vt = eVarType::unknown;
		uint8_t writeMask = 0;
		uint8_t readMask = 0;
	};

	CAtlMap<CStringA, sVarFlags> vars;
	proto.enumBuiltins( [ & ]( const CStringA& name, eVarType vt )
	{
		sVarFlags &vf = vars[ name ];
		vf.builtin = true;
		vf.vt = vt;
	} );

	for( uint8_t i = 0; i < 4; i++ )
	{
		const uint8_t maskBit = (uint8_t)1 << i;
		for( const auto &a : parsed[ i ] )
		{
			sVarFlags& vf = vars[ a.first ];
			vf.writeMask |= maskBit;
			if( vf.vt == eVarType::unknown )
				vf.vt = guessVarType( a.second );
		}
	}

	for( uint8_t i = 0; i < 4; i++ )
	{
		const uint8_t maskBit = (uint8_t)1 << i;
		for( const auto &a : parsed[ i ] )
		{
			enumIdentifiers( a.second, [ & ]( const CStringA& id )
			{
				auto p = vars.Lookup( id );
				if( nullptr != p )
					p->m_value.readMask |= maskBit;
				return false;
			} );
		}
	}

	m_vars.clear();
	m_stateSize = proto.fixedStateSize();

	for( POSITION pos = vars.GetStartPosition(); nullptr != pos; )
	{
		auto p = vars.GetNext( pos );
		const sVarFlags vf = p->m_value;
		if( vf.builtin )
			continue;

		m_vars.emplace_back( sVariable{} );
		sVariable& var = *m_vars.rbegin();
		var.name = p->m_key;
		var.vt = vf.vt;

		const uint8_t usageMask = vf.readMask | vf.writeMask;
		var.usedInState = 0 != ( usageMask & 7 );
		var.usedInFragment = 0 != ( usageMask & 8 );

		if( var.usedInState && var.usedInFragment )
		{
			var.offset = m_stateSize;
			m_stateSize++;
		}
		else
			var.offset = -1;
	}

	return S_OK;
}

UINT Compiler::stateSize()
{
	return (UINT)m_stateSize;
}

const StateShaderTemplate* Compiler::shaderTemplate()
{
	__debugbreak();
	return nullptr;
}