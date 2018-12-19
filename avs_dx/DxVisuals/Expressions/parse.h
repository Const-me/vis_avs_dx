#pragma once
#include "../Hlsl/parseId.hpp"

namespace Expressions
{
	using Assignment = std::pair<CStringA, CStringA>;
	using Assignments = std::vector<Assignment>;

	HRESULT parseAssignments( CStringA code, Assignments& assignments );

	template<class TFunc>
	inline bool enumIdentifiers( const CStringA& exp, TFunc fn )
	{
		using namespace Hlsl;
		
		const int len = exp.GetLength();
		for( int i = 0; i < len; )
		{
			if( !isAlpha( exp[ i ] ) )
			{
				i++;
				continue;
			}

			const int idStart = i;
			for( i++; i < len; )
			{
				if( isAlphaNumeric( exp[ i ] ) )
				{
					i++;
					continue;
				}
				break;
			}
			if( fn( exp.Mid( idStart, i - idStart ) ) )
				return true;
		}
		return false;
	}

	inline CStringA getFirstId( const CStringA& exp )
	{
		CStringA res;
		enumIdentifiers( exp, [ & ]( auto id ) { res = id; return true; } );
		return res;
	}
}