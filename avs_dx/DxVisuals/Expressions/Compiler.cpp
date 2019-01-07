#include "stdafx.h"
#include "Compiler.h"
#include "preprocess.h"
#include "utils.hpp"
#include "parse.h"

CStringA Expressions::expressionMacroName( const char* name )
{
	CStringA res;
	res.Format( "MACRO_%s", name );
	return res;
}

using namespace Expressions;

Compiler::Compiler( const char* effectName, const Prototype& effectPrototype, UINT stateOffset ) :
	m_stateTemplate( effectName ),
	proto( effectPrototype ),
	m_symbols( effectPrototype ),
	m_tree( m_symbols ),
	m_stateOffset( (int)stateOffset )
{
	m_stateTemplate.globals = &m_stateGlobals;
}

HRESULT Compiler::update( const char* init, const char* frame, const char* beat, const char* fragment )
{
	bool somethingChanged = false;
	const array<const char*, 4> nseel{ init, frame, beat, fragment };
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
	for( uint8_t i = 0; i < 3; i++ )
	{
		SILENT_CHECK( parseStatements( m_expressions[ i ], m_tree ) );
		SILENT_CHECK( m_tree.deduceTypes() );
		m_tree.transformDoubleFuncs();
		SILENT_CHECK( m_tree.emitHlsl( m_hlsl[ i ], m_stateTemplate.hasRandomNumbers ) );
		m_tree.getVariablesUsage( m_varUsage, i );
	}

	m_symbols.functions.getStateGlobals( m_stateGlobals );
	m_symbols.functions.clear();

	// Parse and recompile the fragment expression
	SILENT_CHECK( parseStatements( m_expressions[ 3 ], m_tree ) );
	SILENT_CHECK( m_tree.deduceTypes() );
	m_tree.transformDoubleFuncs();
	SILENT_CHECK( m_tree.emitHlsl( m_hlsl[ 3 ], m_fragmentRng ) );
	m_tree.getVariablesUsage( m_varUsage, 3 );
	m_fragmentGlobals = m_symbols.functions.getFragmentGlobals();

	allocateState();
	buildStateHlsl();
	buildFragmentHlsl();
	return S_OK;
}

void Compiler::allocateState()
{
	const int size = (int)m_varUsage.size();

	// Detect variables that are used in both state & fragment expressions, place them in the state
	m_stateSize = proto.fixedStateSize();
	m_dynStateLoad = "";
	m_dynStateStore = "";
	for( int i = 0; i < size; i++ )
	{
		const eVarLocation loc = m_symbols.vars.location( i );
		const uint8_t usage = m_varUsage[ i ].accessMask();
		m_varUsage[ i ].setLocation( loc );

		if( loc == eVarLocation::stateStatic )
		{
			if( usage & 0b10000000 )
				logWarning( "Incorrect variable use: point/vertex expression writing to %s", cstr( m_symbols.vars.name( i ) ) );
			continue;	//< Ignore this var
		}
		bool fragmentOutputWrittenInState = false;

		if( loc != eVarLocation::unknown )
		{
			// Validate access
			switch( loc )
			{
			case eVarLocation::macro:
				if( usage & 0b10101010 )
					logWarning( "Incorrect variable use: writing to constant %s", cstr( m_symbols.vars.name( i ) ) );
				break;
			case eVarLocation::fragmentInput:
				if( usage & 0b10000000 )
					logWarning( "Incorrect variable use: writing to input %s", cstr( m_symbols.vars.name( i ) ) );
			case eVarLocation::fragmentOutput:
				if( usage & 0b00111111 )
				{
					logInfo( "Questionable variable access, %s is output for point/vertex expression", cstr( m_symbols.vars.name( i ) ) );
					fragmentOutputWrittenInState = true;
					m_varUsage[ i ].addExtraFlag( eVarExtraFlag::FragmentOutWrittenInState );
				}
				break;
			}
			if( !fragmentOutputWrittenInState )
				continue;	//< Ignore this var
		}

		if( 0 == usage )
			continue;	//< Ignore this var

		if( !fragmentOutputWrittenInState )
		{
			if( 0 == ( usage & 0b00111111 ) )
			{
				// Only used in fragment expression. Not a good idea to place in the state, fragment expressions run highly parallel.
				m_varUsage[ i ].setLocation( eVarLocation::fragmentLocal );
				continue;	//< Ignore this var
			}
		}

		if( usage & 0b10000000 )
		{
			// This warning means there's data dependency between point/vertex loop iterations.
			// Very hard to fix in a way that wouldn't ruin the performance: GPUs are massively parallel, and loop with dependencies must run sequentially.
			// One workaround that might work well enough in practicce is search for simple updates in fragment expressions, like `x += y` or `x *= y`, and replace them with `x += y * n` or `x *= pow(y,n)`, passing vertex/dispatch thread in a variable.
			logWarning( "Questionable variable access, writing to %s in point/vertex expression", cstr( m_symbols.vars.name( i ) ) );
		}

		m_varUsage[ i ].setLocation( eVarLocation::stateDynamic );
		const int stateOffset = m_stateSize;
		m_stateSize += variableSize( m_symbols.vars.type( i ) );

		// Produce the load/store HLSL pieces for such variables
		const char* name = m_symbols.vars.name( i );
		const eVarType vt = m_symbols.vars.type( i );
		const CStringA load = stateLoad( vt, stateOffset + m_stateOffset );
		m_dynStateLoad.AppendFormat( "		%s = %s;\r\n", name, cstr( load ) );

		const CStringA store = stateStore( vt, stateOffset + m_stateOffset, name );
		m_dynStateStore.AppendFormat( "		%s;\r\n", cstr( store ) );
	}
}

void Compiler::appendMacros( CStringA& hlsl ) const
{
	proto.enumMacros( [ & ]( const VariableDecl& decl )
	{
		if( decl.name == proto.getBeatMacro() )
			return;
		hlsl.AppendFormat( "		%s %s = %s;\r\n", hlslName( decl.vt ), cstr( decl.name ), cstr( expressionMacroName( decl.name ) ) );
	} );
}

void Compiler::buildStateHlsl()
{
	CStringA &code = m_stateTemplate.hlslMain;
	code = "	{\r\n";

	const int size = (int)m_varUsage.size();
	for( int i = 0; i < size; i++ )
	{
		bool isVar = false;
		switch( m_varUsage[ i ].getLocation() )
		{
		case eVarLocation::stateDynamic:
		case eVarLocation::stateLocal:
			isVar = true;
		}
		if( !isVar )
			continue;

		code.AppendFormat( "		%s %s = 0;\r\n", hlslName( m_symbols.vars.type( i ) ), cstr( m_symbols.vars.name( i ) ) );
	}

	const bool stateUsesBeat = ( m_symbols.vars.getBeatMacro() >= 0 ) && ( 0 != ( m_varUsage[ m_symbols.vars.getBeatMacro() ].accessMask() & 0b00111111 ) );
	if( stateUsesBeat )
		code.AppendFormat( "		uint %s = IS_BEAT;", cstr( proto.getBeatMacro() ) );

	appendMacros( code );

	code += "#if INIT_STATE\r\n";
	code += proto.initState();
	code += m_hlsl[ 0 ];	// Init

	code += "#else\r\n";
	code += proto.stateLoad( m_stateOffset );
	code += m_dynStateLoad;

	code += m_hlsl[ 1 ];	// Frame

	const bool hasBeatExpression = m_hlsl[ 2 ].GetLength() > 0;
	if( hasBeatExpression )
	{
		code += "#if IS_BEAT\r\n";
		code += m_hlsl[ 2 ];	// Beat
		code += "#endif\r\n";
	}
	m_stateTemplate.hasBeat = stateUsesBeat || hasBeatExpression;

	code += "#endif\r\n";
	code += proto.stateStore( m_stateOffset );
	code += m_dynStateStore;
	code += "	}\r\n";
}

void Compiler::buildFragmentHlsl()
{
	CStringA &code = m_hlslFragment;
	code = proto.stateLoad( m_stateOffset );

	const int size = (int)m_varUsage.size();
	for( int i = 0; i < size; i++ )
	{
		bool isVar = false;
		switch( m_varUsage[ i ].getLocation() )
		{
		case eVarLocation::stateDynamic:
		case eVarLocation::fragmentLocal:
			isVar = true;
		}
		if( !isVar )
			continue;

		const auto extraFlags = m_varUsage[ i ].extraFlags();
		if( 0 == ( extraFlags & eVarExtraFlag::FragmentOutWrittenInState ) )
			code.AppendFormat( "		%s %s = 0;\r\n", hlslName( m_symbols.vars.type( i ) ), cstr( m_symbols.vars.name( i ) ) );
	}

	m_fragmentBeat = ( m_symbols.vars.getBeatMacro() >= 0 ) && ( 0 != ( m_varUsage[ m_symbols.vars.getBeatMacro() ].accessMask() & 0b11000000 ) );
	if( m_fragmentBeat )
		code.AppendFormat( "		uint %s = IS_BEAT;", cstr( proto.getBeatMacro() ) );
	appendMacros( code );

	code += m_dynStateLoad;

	code += m_hlsl[ 3 ];
}

int Compiler::getIndirectArgOffset( int index ) const
{
	return m_stateOffset + proto.getIndirectArgOffset( index );
}