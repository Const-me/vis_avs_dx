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

HRESULT Compiler::update( const char* init, const char* frame, const char* beat, const char* fragment )
{
	bool somethingChanged = false;
	const std::array<const char*, 4> nseel{ init, frame, beat, fragment };
	CStringA ee;
	for( int i = 0; i < 4; i++ )
	{
		ee = nseel[ i ] ? nseel[ i ] : "";
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
	m_varUsage.clear();

	m_stateGlobals.clear();
	m_stateTemplate.hasBeat = false;
	m_stateTemplate.hlslMain = "";
	m_hlslFragment = "";
	m_stateSize = proto.fixedStateSize();
	m_stateTemplate.hasRandomNumbers = false;
	m_fragmentRng = false;
	if( !hasAny( m_expressions, []( const CStringA& s ) { return s.GetLength() > 0; } ) )
	{
		// All expressions are empty
		return S_OK;
	}

	// TODO [optimization]: find sub-expressions of the fragment expression that don't depend on per-fragment inputs, and move these calculations to the state shader.
	// This is only worth doing for sub-expressions that are complex enough, e.g. use sin_d/cos_d, or getspec/getosc calls.

	// Parse and recompile the state expressions
	for( int i = 0; i < 3; i++ )
	{
		CHECK( parseAssignments( m_expressions[ i ], m_tree ) );
		CHECK( m_tree.deduceTypes() );
		m_tree.transformDoubleFuncs();
		CHECK( m_tree.emitHlsl( m_hlsl[ i ], m_stateTemplate.hasRandomNumbers ) );
		m_tree.getVariablesUsage( m_varUsage, false );
	}

	m_symbols.functions.getStateGlobals( m_stateGlobals );
	m_symbols.functions.clear();

	// Parse and recompile the fragment expression
	CHECK( parseAssignments( m_expressions[ 3 ], m_tree ) );
	CHECK( m_tree.deduceTypes() );
	m_tree.transformDoubleFuncs();
	CHECK( m_tree.emitHlsl( m_hlsl[ 3 ], m_fragmentRng ) );
	m_tree.getVariablesUsage( m_varUsage, true );
	m_fragmentGlobals = m_symbols.functions.getFragmentGlobals();

	CHECK( allocateState() );
	CHECK( buildStateHlsl() );
	CHECK( buildFragmentHlsl() );
	return S_OK;
}

HRESULT Compiler::allocateState()
{
	const int size = (int)m_varUsage.size();

	// Detect variables that are used in both state & fragment expressions, place them in the state
	m_stateSize = proto.fixedStateSize();
	m_dynStateLoad = "";
	m_dynStateStore = "";
	for( int i = 0; i < size; i++ )
	{
		const eVarLocation loc = m_symbols.vars.location( i );
		const uint8_t usage = m_varUsage[ i ];

		if( loc == eVarLocation::stateStatic )
		{
			if( usage & 0b1000 )
				logWarning( "Incorrect variable use: point/vertex expression writing to %s", cstr( m_symbols.vars.name( i ) ) );
			continue;
		}

		if( loc != eVarLocation::unknown )
		{
			// Validate access
			switch( loc )
			{
			case eVarLocation::macro:
				if( usage & 0b1010 )
					logWarning( "Incorrect variable use: writing to constant %s", cstr( m_symbols.vars.name( i ) ) );
				break;
			case eVarLocation::fragmentInput:
				if( usage & 0b1000 )
					logWarning( "Incorrect variable use: writing to input %s", cstr( m_symbols.vars.name( i ) ) );
			case eVarLocation::fragmentOutput:
				if( usage & 0b0011 )
					logWarning( "Incorrect variable use: %s is only accessible in point/vertex expression", cstr( m_symbols.vars.name( i ) ) );
				break;
			}
			continue;
		}

		if( 0 == usage )
			continue;

		if( 0 == ( usage & 0b1100 ) )
		{
			m_symbols.vars.setLocation( i, eVarLocation::stateLocal );
			continue;
		}
		if( 0 == ( usage & 0b0011 ) )
		{
			m_symbols.vars.setLocation( i, eVarLocation::fragmentLocal );
			continue;
		}
		if( usage & 0b1000 )
		{
			// This warning means there's data dependency between point/vertex loop iterations.
			// Very hard to fix in a way that wouldn't ruin the performance: GPUs are massively parallel, and loop with dependencies must run sequentially.
			// One workaround that might work well enough in practicce is search for simple updates in fragment expressions, like `x += y` or `x *= y`, and replace them with `x += y * n` or `x *= pow(y,n)`, passing vertex/dispatch thread in a variable.
			logWarning( "Incorrect variable use: writing to %s in point/vertex expression", cstr( m_symbols.vars.name( i ) ) );
		}

		m_symbols.vars.setLocation( i, eVarLocation::stateDynamic );
		const int stateOffset = m_stateSize;
		m_stateSize += variableSize( m_symbols.vars.type( i ) );

		// Produce the load/store HLSL pieces for such variables
		const char* name = m_symbols.vars.name( i );
		const eVarType vt = m_symbols.vars.type( i );
		const CStringA load = stateLoad( vt, stateOffset );
		m_dynStateLoad.AppendFormat( "		%s = %s;\r\n", name, cstr( load ) );

		const CStringA store = stateStore( vt, stateOffset, name );
		m_dynStateStore.AppendFormat( "		%s;\r\n", cstr( store ) );
	}

	return S_OK;
}

HRESULT Compiler::buildStateHlsl()
{
	CStringA &code = m_stateTemplate.hlslMain;
	code = "	{\r\n";
	const int size = (int)m_varUsage.size();
	for( int i = 0; i < size; i++ )
	{
		bool isVar = false;
		switch( m_symbols.vars.location( i ) )
		{
		case eVarLocation::stateDynamic:
		case eVarLocation::stateLocal:
			isVar = true;
		}
		if( !isVar )
			continue;
		
		code.AppendFormat( "		%s %s;\r\n", hlslName( m_symbols.vars.type( i ) ), cstr( m_symbols.vars.name( i ) ) );
	}

	code += "#if INIT_STATE\r\n";
	code += proto.initState();
	code += m_hlsl[ 0 ];	// Init

	code += "#else\r\n";
	code += proto.stateLoad();
	code += m_dynStateLoad;
	
	code += m_hlsl[ 1 ];	// Frame

	m_stateTemplate.hasBeat = m_hlsl[ 2 ].GetLength() > 0;
	if( m_stateTemplate.hasBeat )
	{
		code += "#if IS_BEAT\r\n";
		code += m_hlsl[ 2 ];	// Beat
		code += "#endif\r\n";
	}
	code += "#endif\r\n";
	code += proto.stateStore();
	code += m_dynStateStore;
	code += "	}\r\n";

	return S_OK;
}

HRESULT Compiler::buildFragmentHlsl()
{
	CStringA &code = m_hlslFragment;
	code = proto.stateLoad();

	const int size = (int)m_varUsage.size();
	for( int i = 0; i < size; i++ )
	{
		bool isVar = false;
		switch( m_symbols.vars.location( i ) )
		{
		case eVarLocation::stateDynamic:
		case eVarLocation::fragmentLocal:
			isVar = true;
		}
		if( !isVar )
			continue;

		code.AppendFormat( "		%s %s;\r\n", hlslName( m_symbols.vars.type( i ) ), cstr( m_symbols.vars.name( i ) ) );
	}

	code += m_dynStateLoad;

	code += m_hlsl[ 3 ];

	return S_OK;
}