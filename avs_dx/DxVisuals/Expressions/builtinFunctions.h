#pragma once
#include "eVarType.h"

namespace Expressions
{
	bool isBuiltinFunction( const CStringA& func );

	bool isBuiltinFunction( const CStringA& func, eVarType& vt );
}