#include "stdafx.h"
#include "builtinFunctions.h"

namespace Expressions
{
	struct BuiltinMap
	{
	protected:
		CAtlMap<CStringA, eVarType> map;

	public:
		bool lookup( const CStringA& func, eVarType& vt ) const
		{
			auto p = map.Lookup( func );
			if( nullptr != p )
			{
				vt = p->m_value;
				return true;
			}
			vt = eVarType::unknown;
			return false;
		}

		bool lookup( const CStringA& func ) const
		{
			return nullptr != map.Lookup( func );
		}
	};
	struct BuiltinFunctions: public BuiltinMap
	{
		BuiltinFunctions();
	};
	struct DoubleFunctions : public BuiltinMap
	{
		DoubleFunctions();
	};

	static const BuiltinFunctions s_builtin;
	static const DoubleFunctions s_doubles;

	bool isBuiltinFunction( const CStringA& func, eVarType& vt )
	{
		return s_builtin.lookup( func, vt );
	}

	bool isBuiltinFunction( const CStringA& func )
	{
		return s_builtin.lookup( func );
	}

	bool hasDoubleVersion( const CStringA& func, eVarType& vt )
	{
		return s_doubles.lookup( func, vt );
	}

	bool hasDoubleVersion( const CStringA& func )
	{
		return s_doubles.lookup( func );
	}

	BuiltinFunctions::BuiltinFunctions()
	{
		// Actually, min & max aren't necessarily float, they can be int32, too. Default to float because it's harder to implement polymorphic ones, and floats are faster.
		const char* floatFuncs[] = { "abs", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "sqrt", "pow", "exp", "log", "log10", "floor", "ceil", "min", "max" };
		for( const char* ff : floatFuncs )
			map[ ff ] = eVarType::f32;

		const char* intFuncs[] = { "sign" };
		for( const char* ff : intFuncs )
			map[ ff ] = eVarType::i32;

		map.Rehash();
	}

	DoubleFunctions::DoubleFunctions()
	{
		const char* floatFuncs[] = { "sin", "cos", "tan" };
		for( const char* ff : floatFuncs )
			map[ ff ] = eVarType::f32;

		const char* doubleFuncs[] = { "abs", "floor", "ceil", "min", "max" };
		for( const char* ff : doubleFuncs )
			map[ ff ] = eVarType::f64;

		map.Rehash();
	}
}