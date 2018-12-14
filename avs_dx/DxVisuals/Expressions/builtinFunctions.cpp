#include "stdafx.h"
#include "builtinFunctions.h"

namespace Expressions
{
	struct BuiltinFunctions
	{
		CAtlMap<CStringA, eVarType> map;

		BuiltinFunctions();
	};

	static const BuiltinFunctions s_builtin;

	bool isBuiltinFunction( const CStringA& func, eVarType& vt )
	{
		auto p = s_builtin.map.Lookup( func );
		if( nullptr != p )
		{
			vt = p->m_value;
			return true;
		}
		return false;
	}

	bool isBuiltinFunction( const CStringA& func )
	{
		return nullptr != s_builtin.map.Lookup( func );
	}

	BuiltinFunctions::BuiltinFunctions()
	{
		const char* floatFuncs[] = { "abs", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "sqrt", "pow", "exp", "log", "log10", "floor", "ceil", "sign", "min", "max" };
		for( const char* ff : floatFuncs )
			map[ ff ] = eVarType::f32;
		map.Rehash();
	}
}