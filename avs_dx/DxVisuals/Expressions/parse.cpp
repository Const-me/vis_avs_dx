#include "stdafx.h"
#include "parse.h"
#include "utils.hpp"
#include "Tree/Tree.h"

namespace Expressions
{
	HRESULT parseStatement( const CStringA& nseel, Expressions::Tree& tree, int begin, int equals, int end );
}

HRESULT Expressions::parseStatement( const CStringA& nseel, Expressions::Tree& tree, int begin, int equals, int end )
{
	if( end <= begin )
		return S_FALSE;
	if( equals < 0 )
		return tree.appendStatement( nseel, begin, end );

	if( equals + 1 >= end )
	{
		logWarning( "Error in the expression, '=' without the right hand side." );
		return S_FALSE;
	}
	return tree.appendAssignment( nseel, begin, equals, end );
}

HRESULT Expressions::parseStatements( const CStringA& nseel, Expressions::Tree& tree )
{
	tree.clear();

	int begin = 0;
	int equals = -1;
	const int end = nseel.GetLength();
	bool error = false;
	for( int i = 0; i < end; i++ )
	{
		const char c = nseel[ i ];
		if( c != ';' && c != '=' )
			continue;
		if( c == '=' )
		{
			if( equals >= 0 )
			{
				logWarning( "Error in the expression: too many '=' in the statement." );
				error = true;
			}
			else if( i == begin )
			{
				logWarning( "Error in the expression, '=' must be preceded by a variable." );
				begin = i + 1;
			}
			else
				equals = i;
			continue;
		}
		// Found the semicolon.
		if( !error )
			SILENT_CHECK( parseStatement( nseel, tree, begin, equals, i ) );
		error = false;
		equals = -1;
		begin = i + 1;
	}
	if( !error )
		SILENT_CHECK( parseStatement( nseel, tree, begin, equals, end ) );

	return S_OK;
}