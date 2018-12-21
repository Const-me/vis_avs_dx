#pragma once
#include "eVarType.h"

namespace Expressions
{
	class SymbolTable
	{
		std::vector<CStringA> names;

		struct Variable
		{
			int id;
			eVarType vt = eVarType::unknown;
		};

		CAtlMap<CStringA, Variable> variables;

		enum eFunctionKind : uint8_t
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
			int id;
			eFunctionKind kind = eFunctionKind::unknown;
			eVarType vt = eVarType::unknown;
		};

		CAtlMap<CStringA, Function> functions;

		void addInternalFunc( const CStringA& name );

	public:
		SymbolTable();

		// Insert or lookup a variable
		int varLookup( const CStringA& name, eVarType& vt );

		// Insert or lookup a function
		int funcLookup( const CStringA& name, eVarType &vt );
	};
}