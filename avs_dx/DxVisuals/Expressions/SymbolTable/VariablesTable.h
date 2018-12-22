#pragma once
#include "../VariableDecl.hpp"
#include "../Prototype.h"

namespace Expressions
{
	class VariablesTable
	{
		std::vector<VariableDecl> table;
		CAtlMap<CStringA, int> map;
		int prototypeSize = 0;

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

		eVarLocation location( int id ) const
		{
			return table[ id ].loc;
		}

		int sizePrototype() const
		{
			return prototypeSize;
		}

		int size() const
		{
			return (int)table.size();
		}

		void clearLocals();
	};
}