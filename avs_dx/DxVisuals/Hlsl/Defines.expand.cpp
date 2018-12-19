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
		const char* const src = hlsl;
		ids.clear();

		// Find the identifiers, collect macro keys into the vector
		enumIdentifiers( hlsl, [ =, &ids ]( int idStart, int idLength )
		{
			if( idLength != macroLen )
				return false;
			if( 0 != strncmp( src + idStart, m.first, (size_t)idLength ) )
				return false;
			// Found the macro we're looking for
			ids.push_back( idStart );
			return false;
		} );

		if( ids.empty() )
			continue;	// Not found any

		// Replace them
		const int expandLengthDiff = m.second.GetLength() - macroLen;

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