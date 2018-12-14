#include "stdafx.h"
#include "comments.h"

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
			logError( "fatal error C1071: unexpected end of file found in comment" );
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

	return S_OK;
}