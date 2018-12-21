#pragma once
#include "VariableDecl.hpp"
#include "Prototype.h"

namespace Expressions
{
	enum struct eFunctionKind : uint8_t
	{
		unknown,
		// HLSL built-in, e.g. "floor" or "atan2"
		Hlsl,
		// AVS built-in from shaderFunctions.hlsl, e.g. "gettime" or "above"
		Avs,
		// Can be either, depends on argument types
		Polymorphic,
		// Internal to expression engine, e.g. "if" compiles to `operator ?`, "equals" to `operator ==`, etc.
		Internal
	};

	// Internal functions have their IDs hardcoded, the ID values are equal to the values in this enum (excluding the valuesCount)
	enum eInternalFunc: int
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

	class SymbolTable
	{
		std::vector<VariableDecl> variables;
		CAtlMap<CStringA, int> variablesMap;
		
		struct Function: public FunctionType
		{
			CStringA name;
		};
		std::vector<Function> functions;
		CAtlMap<CStringA, int> functionsMap;

		int addFunc( const CStringA& name, eFunctionKind kind, eVarType vt = eVarType::unknown );

		void addInternals();

	public:

		SymbolTable();
		SymbolTable( const Prototype& proto );

		// Insert or lookup a variable
		int varLookup( const CStringA& name, eVarType& vt );

		// Insert or lookup a function
		int funcLookup( const CStringA& name, eVarType &vt );

		eVarType varGetType( int id ) const;
		eVarType varSetType( int id, eVarType vt );
		const CStringA& varName( int id ) const
		{
			return variables[ id ].name;
		}

		FunctionType funGetType( int id ) const;
		const CStringA& funcName( int id ) const
		{
			return functions[ id ].name;
		}

		size_t variablesCount() const
		{
			return variables.size();
		}
	};
}