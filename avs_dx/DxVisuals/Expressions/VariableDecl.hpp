#pragma once
#include "varEnums.h"

namespace Expressions
{
	struct VariableDecl
	{
		eVarLocation loc = eVarLocation::unknown;
		eVarType vt = eVarType::unknown;
		CStringA name;

		VariableDecl() = default;
		VariableDecl( const VariableDecl& ) = default;
		VariableDecl( eVarLocation c, eVarType t, const CStringA& n ) :
			loc( c ), vt( t ), name( n ) { }
	};
}