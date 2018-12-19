#include "stdafx.h"
#include "parse.h"
#include "utils.hpp"

namespace Expressions
{
	// Remove both C and C++ comments
	HRESULT removeComments( CStringA& code );

	// If the expression is x=y, return the index of the '=' character, otherwise S_FALSE
	HRESULT isAssign( const CStringA& expr, int& indEqual );

	const char* expandDollarMacro( const CStringA& m );
}

HRESULT Expressions::removeComments( CStringA& code )
{
	// C-style comments
	for( int i = 0; true; )
	{
		i = code.Find( "/*", i );
		if( i < 0 )
			break;
		const int j = code.Find( "*/", i + 2 );
		if( j < 0 )
		{
			logError( "Syntax error, unexpected end of file found in comment" );
			return E_INVALIDARG;
		}
		code = code.Left( i ) + code.Mid( j + 2 );
	}

	// C++ style comments
	for( int i = 0; true; )
	{
		i = code.Find( "//", i );
		if( i < 0 )
			break;
		const int j = code.Find( '\n', i + 2 );
		if( j < 0 )
		{
			code = code.Left( i );
			break;
		}
		code = code.Left( i ) + code.Mid( j + 1 );
	}

	// Couple of macros
	using namespace Hlsl;
	for( int i = 0; true; )
	{
		i = code.Find( '$', i );
		if( i < 0 )
			break;
		const int dollar = i;
		for( i++; isAlphaNumeric( code[ i ] ); i++ ) {}

		const CStringA m = code.Mid( dollar + 1, i - dollar - 1 );
		const char* v = expandDollarMacro( m );
		if( nullptr == v )
		{
			logError( "Syntax error, unknown macro $%s", cstr( m ) );
			return E_INVALIDARG;
		}
		code = code.Left( dollar ) + v + code.Mid( i );
		i = dollar + m.GetLength();
	}

	return S_OK;
}

const char* Expressions::expandDollarMacro( const CStringA& m )
{
	if( m == "PI" ) return "constantPi()";
	if( m == "E" ) return "constantE()";
	if( m == "PHI" ) return "constantGoldenRatio()";
	return nullptr;
}

HRESULT Expressions::isAssign( const CStringA& expr, int& idx )
{
	idx = expr.Find( '=' );
	if( idx < 0 )
		return S_FALSE;
	if( 0 == idx )
	{
		logError( "Syntax error, '=' in expression %s", cstr( expr.Left( idx ) ) );
		return E_INVALIDARG;
	}
	if( !isAlpha( expr[ 0 ] ) )
	{
		logError( "Syntax error: '=', invalid left operand %s", cstr( expr.Left( idx ) ) );
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT Expressions::parseAssignments( CStringA code, Assignments& assignments )
{
	assignments.clear();
	if( code.GetLength() <= 0 )
		return S_FALSE;

	CHECK( removeComments( code ) );

	// Filter out whitespace
	code.Remove( ' ' );
	code.Remove( '\t' );
	code.Remove( '\r' );
	code.Remove( '\n' );

	if( code.GetLength() <= 0 )
		return S_FALSE;

	// Parse into expressions
	CStringA exp;
	for( int i = 0; true; )
	{
		exp = code.Tokenize( ";", i );
		if( exp.GetLength() <= 0 )
			break;

		int indEqual;
		const HRESULT hr = isAssign( exp, indEqual );
		CHECK( hr );
		if( S_FALSE == hr )
		{
			logWarning( "Unknown expression \"%s\" - ignoring", cstr( exp ) );
			continue;
		}
		assignments.emplace_back( std::make_pair( exp.Left( indEqual ), exp.Mid( indEqual + 1 ) ) );
	}

	return S_OK;
}