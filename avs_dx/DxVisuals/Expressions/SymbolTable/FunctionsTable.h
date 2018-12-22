#pragma once
#include "../varEnums.h"

namespace Expressions
{
	enum struct eFunctionKind : uint8_t
	{
		unknown,
		// HLSL built-in, e.g. "floor" or "atan2"
		Hlsl,
		// AVS built-in from ../Builtin/*.hlsl, e.g. "gettime" or "above"
		Avs,
		// Can be either, depends on argument types
		Polymorphic,
		// Internal to expression engine, e.g. "if" compiles to `operator ?`, "equals" to `operator ==`, etc.
		Internal
	};

	// Internal functions have their IDs hardcoded, the ID values are equal to the values in this enum (excluding the valuesCount)
	enum eInternalFunc : int
	{
		Assign,
		Equals,
		If,
		Rand,
		valuesCount,
	};

	struct FunctionType
	{
		eFunctionKind kind = eFunctionKind::unknown;
		eVarType vt = eVarType::unknown;
	};

	class FunctionsTable
	{
		struct Function : public FunctionType
		{
			CStringA name;
		};
		std::vector<Function> table;
		CAtlMap<CStringA, int> map;

		int add( const CStringA& name, eFunctionKind kind, eVarType vt = eVarType::unknown );

		void addInternals();

		template<class TFunc>
		void enumAvsFuncs( TFunc callback ) const;

	public:

		FunctionsTable();

		int addAvs( const CStringA& name );
		int addAvs( const CStringA& name, eVarType &vt );

		// Insert or lookup a function
		int lookup( const CStringA& name, eVarType &vt );

		FunctionType type( int id ) const;

		const CStringA& name( int id ) const;

		// Get code of AVS functions, 1 string per function
		void getStateGlobals( std::vector<CStringA>& list ) const;

		// Get code of AVS functions, joined with newlines into a single string
		CStringA getFragmentGlobals() const;

		// Drop all functions from the table, only leave internal ones.
		void clear();
	};
}