#pragma once
#include "../VariableDecl.hpp"
#include "../Prototype.h"

namespace Expressions
{
	class VariablesTable
	{
		std::vector<VariableDecl> table;
		CAtlMap<CStringA, int> map;

	public:

		VariablesTable();
		VariablesTable( const Prototype& proto );

		// Insert or lookup a variable
		int lookup( const CStringA& name, eVarType& vt );

		eVarType type( int id ) const;
		eVarType setType( int id, eVarType vt );

		const CStringA& name( int id ) const
		{
			return table[ id ].name;
		}

		size_t size() const
		{
			return table.size();
		}
	};
}