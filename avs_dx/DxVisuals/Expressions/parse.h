#pragma once
#include "../Hlsl/parseId.hpp"

namespace Expressions
{
	using Assignment = std::pair<CStringA, CStringA>;
	using Assignments = std::vector<Assignment>;

	HRESULT parseAssignments( CStringA code, Assignments& assignments );

	template<class TFunc>
	inline bool findIdentifier( const CStringA& exp, TFunc fnGotId )
	{
		return Hlsl::enumIdentifiers( exp, [ =, &exp ]( int start, int length ) { return fnGotId( exp.Mid( start, length ) ); } );
	}

	inline CStringA getFirstId( const CStringA& exp )
	{
		CStringA res;
		findIdentifier( exp, [ & ]( const CStringA& id ) { res = id; return true; } );
		return res;
	}
}