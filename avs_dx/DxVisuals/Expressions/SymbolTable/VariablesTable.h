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
		int m_beatMacro = -1;

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

		void setLocation( int id, eVarLocation loc )
		{
			assert( table[ id ].loc == eVarLocation::unknown );
			table[ id ].loc = loc;
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

		void defaultTypesToFloat();

		// If the prototype defines beat macro, will return the ID of the variable. Otherwise, -1.
		int getBeatMacro() const { return m_beatMacro; }
	};
}