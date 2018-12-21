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
	m_miscFlags = eMiscFlags::None;
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
		if( n.id != eInternalFunc::Rand )
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
	if( any )
		m_miscFlags = (eMiscFlags)( m_miscFlags | eMiscFlags::HasRandom );
	return any;
}

bool Tree::transformDoubleFuncs()
{
	bool any = false;
	const int size = (int)m_nodes.size();
	for( int i = 0; i < size; i++ )
	{
		Node& n = m_nodes[ i ];
		if( n.node != eNode::Func )
			continue;
		const FunctionType ft = symbols.funGetType( n.id );
		if( ft.kind != eFunctionKind::Polymorphic )
			continue;

		// Found a polymorphic function
		bool hasDoubleArg = false;
		for( int j = i + 1; j >= 0;)
		{
			const Node& a = m_nodes[ j ];
			if( a.vt == eVarType::f64 )
			{
				hasDoubleArg = true;
				break;
			}
			j = a.nextSibling;
		}
		if( !hasDoubleArg )
			continue;

		// It accepts double argument
		CStringA name = symbols.funcName( n.id );
		name += "_d";
		n.id = symbols.funcLookup( name, n.vt );
#ifdef DEBUG
		n.source = name;
#endif
		any = true;
	}
	if( any )
		m_miscFlags = (eMiscFlags)( m_miscFlags | eMiscFlags::HasDoubleFunc );
	return any;
}