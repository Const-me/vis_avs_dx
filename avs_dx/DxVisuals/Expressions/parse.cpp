#include "stdafx.h"
#include "parse.h"
#include "utils.hpp"
#include "Tree/Tree.h"

namespace Expressions
{

	// If the expression is x=y, return the index of the '=' character, otherwise S_FALSE
	HRESULT isAssign( const CStringA& expr, int& indEqual );

	const char* expandDollarMacro( const CStringA& m );
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

HRESULT Expressions::parseAssignments( const CStringA& code, Expressions::Tree& tree )
{
	tree.clear();
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

		const CStringA lhs = exp.Left( indEqual );
		const CStringA rhs = exp.Mid( indEqual + 1 );

		CHECK( tree.appendAssignment( lhs, rhs ) );
	}
	return S_OK;
}