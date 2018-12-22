#include "stdafx.h"
#include "Compiler.h"
#include "preprocess.h"
#include "utils.hpp"
#include "parse.h"

using namespace Expressions;

Compiler::Compiler( const char* effectName, const Prototype& effectPrototype ) :
	m_stateTemplate( effectName ),
	proto( effectPrototype ),
	m_symbols( effectPrototype ),
	m_tree( m_symbols )
{
	m_stateTemplate.globals = &m_stateGlobals;
}

HRESULT Compiler::update( RString effect_exp[ 4 ] )
{
	bool somethingChanged = false;
	CStringA ee;
	for( int i = 0; i < 4; i++ )
	{
		ee = effect_exp[ i ].get();
		preprocess( ee );
		if( ee == m_expressions[ i ] )
			continue;
		m_expressions[ i ] = ee;
		somethingChanged = true;
	}
	if( !somethingChanged )
		return S_FALSE;

	m_symbols.clear();
	m_tree.clear();
	m_stateUsage.clear();
	m_fragmentUsage.clear();

	m_stateGlobals.clear();
	m_stateTemplate.hasBeat = false;
	m_stateTemplate.hlslMain = "";
	m_hlslFragment = "";
	m_stateSize = proto.fixedStateSize();
	if( !hasAny( m_expressions, []( const CStringA& s ) { return s.GetLength() > 0; } ) )
	{
		// All expressions are empty
		return S_OK;
	}

	std::array<int, 3> stateExpressions = { 3, 1, 2 };
	bool stateUsesRng = false;
	for( int e : stateExpressions )
	{
		CHECK( parseAssignments( m_expressions[ e ], m_tree ) );
		m_tree.transformDoubleFuncs();
		CHECK( m_tree.emitHlsl( m_hlsl[ e ], stateUsesRng ) );
		m_tree.getVariableUsage( m_stateUsage );
	}

	m_symbols.functions.getStateGlobals( m_stateGlobals );
	m_symbols.functions.clear();

	CHECK( parseAssignments( m_expressions[ 0 ], m_tree ) );
	m_tree.transformDoubleFuncs();
	bool fragmentUsesRng = false;
	CHECK( m_tree.emitHlsl( m_hlsl[ 0 ], fragmentUsesRng ) );
	m_tree.getVariableUsage( m_fragmentUsage );

	m_fragmentGlobals = m_symbols.functions.getFragmentGlobals();

	return E_NOTIMPL;
}

/* void Compiler::printLoadState( CStringA& code ) const
{
	for( const auto &v : m_vars )
		if( v.offset >= 0 )
			code.AppendFormat( "\t\t%s = %s;\r\n", cstr( v.name ), cstr( stateLoad( v.vt, v.offset ) ) );
}

void Compiler::printStoreState( CStringA& code ) const
{
	for( const auto &v : m_vars )
		if( v.offset >= 0 )
			code.AppendFormat( "\t\t%s;\r\n", cstr( stateStore( v.vt, v.offset, v.name ) ) );
}

HRESULT Compiler::buildStateCode( const std::array<Assignments, 4>& parsed )
{
	CStringA code = "	{\r\n";
	for( const auto &v : m_vars )
		if( v.usedInState )
			code.AppendFormat( "		%s %s;\r\n", hlslName( v.vt ), cstr( v.name ) );

	code += "#if INIT_STATE\r\n";
	code += proto.initState();
	printAssignments( code, parsed[ 3 ] );

	code += "#else\r\n";
	code += proto.stateLoad();
	printLoadState( code );

	printAssignments( code, parsed[ 1 ] );

	m_stateTemplate.hasBeat = !parsed[ 2 ].empty();
	if( m_stateTemplate.hasBeat )
	{
		code += "#if IS_BEAT\r\n";
		printAssignments( code, parsed[ 2 ] );
		code += "#endif\r\n";
	}
	code += "#endif\r\n";
	code += proto.stateStore();
	printStoreState( code );
	code += "	}\r\n";

	m_stateTemplate.hlslMain = code;
	return S_OK;
}

HRESULT Compiler::buildFragmentCode( const Assignments& parsed )
{
	CStringA code = proto.stateLoad();
	for( const auto &v : m_vars )
	{
		if( !v.usedInFragment )
			continue;
		if( v.offset >= 0 )
		{
			const CStringA load = Expressions::stateLoad( v.vt, v.offset );
			code.AppendFormat( "		%s %s = %s;\r\n", hlslName( v.vt ), cstr( v.name ), cstr( load ) );
		}
		else
			code.AppendFormat( "		%s %s;\r\n", hlslName( v.vt ), cstr( v.name ) );
	}
	printAssignments( code, parsed );
	m_hlslFragment = code;
	return S_OK;
} */