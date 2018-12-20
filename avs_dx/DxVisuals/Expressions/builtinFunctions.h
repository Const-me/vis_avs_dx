#pragma once
#include "eVarType.h"

namespace Expressions
{
	// true if this function is HLSL built-in one
	bool isBuiltinFunction( const CStringA& func );

	// true if this function is HLSL built-in one, also get the return type of that function
	bool isBuiltinFunction( const CStringA& func, eVarType& vt );

	// true if we have custom function that accepts double argument.
	bool hasDoubleVersion( const CStringA& func );
	bool hasDoubleVersion( const CStringA& func, eVarType& vt );
}