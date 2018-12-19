#include "stdafx.h"
#include "Defines.h"
#include "parseId.hpp"

using namespace Hlsl;

CStringA Defines::expand( CStringA hlsl ) const
{
	std::vector<int> ids;

	for( const auto& m : m_map )
	{
		const int macroLen = m.first.GetLength();

		// Find the identifiers, collect them in the vector
		const char* const src = hlsl;
		ids.clear();
		const int len = hlsl.GetLength();
		for( int i = 0; i < len; )
		{
			if( !isAlpha( src[ i ] ) )
			{
				i++;
				continue;
			}
			const int idStart = i;

			for( i++; i < len; )
			{
				if( isAlphaNumeric( src[ i ] ) )
				{
					i++;
					continue;
				}
				break;
			}
			const int idLength = i - idStart;

			if( idLength != macroLen )
				continue;
			if( 0 != strncmp( src + idStart, m.first, (size_t)idLength ) )
				continue;
			// Found the macro
			ids.push_back( idStart );
		}

		if( ids.empty() )
			continue;	// Not found any

		// Replace them
		const int valueLen = m.second.GetLength();
		const int expandLengthDiff = valueLen - macroLen;

		int offset = 0;
		for( int i : ids )
		{
			const int pos = i + offset;
			hlsl = hlsl.Left( pos ) + m.second + hlsl.Mid( pos + macroLen );
			offset += expandLengthDiff;
		}
	}
	return hlsl;
}