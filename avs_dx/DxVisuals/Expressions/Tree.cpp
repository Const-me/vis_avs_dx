#include "stdafx.h"
#include "Tree.h"
using namespace Expressions;

Tree::Tree( SymbolTable& symbolsTable ) :
	symbols( symbolsTable )
{ }

void Tree::clear()
{
	m_lastStatement = -1;
	m_codez.clear();
	m_nodes.clear();
}

bool Tree::transformRandoms()
{
	bool any = false;
	int size = (int)m_nodes.size();
	for( int i = 0; i < size; i++ )
	{
		Node& n = m_nodes[ i ];
		if( n.node != eNode::Func )
			continue;
		if( n.id != SymbolTable::idRand )
			continue;
		assert( 1 == n.length );

		// Insert a new argument
		Node nn;
		nn.node = eNode::Var;
		nn.vt = eVarType::u32;
#ifdef DEBUG
		nn.source = "rng_state";
#endif
		nn.id = symbols.varLookup( "rng_state", nn.vt );
		m_nodes.insert( m_nodes.begin() + i + 1, nn );

		// Fix siblings broken after the insert
		for( Node& n : m_nodes )
		{
			if( n.nextSibling > i )
				n.nextSibling++;
		}

		// Set sibling of the new argument
		m_nodes[ i + 1 ].nextSibling = i + 2;

		// Replace the function
		n.id = symbols.funcLookup( "rand_avs", n.vt );
#ifdef DEBUG
		n.source = "rand_avs";
#endif
		n.length++;

		size++;
		any = true;
	}
	return any;
}