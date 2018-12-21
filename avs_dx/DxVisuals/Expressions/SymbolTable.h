#pragma once
#include "eVarType.h"
#include "Prototype.h"

namespace Expressions
{
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

	struct FunctionType
	{
		eFunctionKind kind = eFunctionKind::unknown;
		eVarType vt = eVarType::unknown;
	};

	class SymbolTable
	{
		struct Variable
		{
			CStringA name;
			eVarType vt = eVarType::unknown;
		};
		std::vector<Variable> variables;
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

		static constexpr int idAssign = 0;
		static constexpr int idEquals = 1;
		static constexpr int idIf = 2;
		static constexpr int idRand = 3;

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
	};
}