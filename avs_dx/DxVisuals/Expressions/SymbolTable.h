#pragma once
#include "eVarType.h"

namespace Expressions
{
	class SymbolTable
	{
		struct Variable
		{
			CStringA name;
			eVarType vt = eVarType::unknown;
		};
		std::vector<Variable> variables;
		CAtlMap<CStringA, int> variablesMap;

		enum struct eFunctionKind : uint8_t
		{
			unknown,
			// HLSL built-in
			Hlsl,
			// AVS built-in, from shaderFunctions.hlsl
			Avs,
			// Can be either, depends on argument types
			Polymorphic,
			// Internal to expression engine: "if" compiles to operator ?
			Internal
		};
		struct Function
		{
			CStringA name;
			eFunctionKind kind = eFunctionKind::unknown;
			eVarType vt = eVarType::unknown;
		};
		std::vector<Function> functions;
		CAtlMap<CStringA, int> functionsMap;

		int addFunc( const CStringA& name, eFunctionKind kind, eVarType vt );
		int addInternalFunc( const CStringA& name );

	public:

		SymbolTable();

		// Insert or lookup a variable
		int varLookup( const CStringA& name, eVarType& vt );

		// Insert or lookup a function
		int funcLookup( const CStringA& name, eVarType &vt );
	};
}