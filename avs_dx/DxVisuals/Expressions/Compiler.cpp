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

	for( int i = 0; i < 4; i++ )
		CHECK( parseAssignments( m_expressions[ i ], m_parsed[ i ] ) );

	CHECK( deductTypes() );

	return E_NOTIMPL;
}

HRESULT Compiler::deductTypes()
{
	return E_NOTIMPL;
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