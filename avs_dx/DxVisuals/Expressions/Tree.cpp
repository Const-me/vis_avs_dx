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
	return any;
}