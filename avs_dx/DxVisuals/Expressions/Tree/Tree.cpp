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
		const FunctionType ft = symbols.functions.type( n.id );
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
		if( ft.vt == eVarType::f32 )
		{
			// It's sin/cos/tan, inject extra dependency before that.
			symbols.functions.addAvs( "wrap_double_2pi" );
		}

		CStringA name = symbols.functions.name( n.id );
		name += "_d";
		n.id = symbols.functions.addAvs( name, n.vt );
#ifdef DEBUG
		n.source = name;
#endif
		any = true;
	}
	return any;
}

void Tree::getVariablesUsage( std::vector<uint8_t>& usage, uint8_t nExpressionBlock ) const
{
	usage.resize( symbols.vars.size(), 0 );
	assert( nExpressionBlock < 4 );
	const uint8_t shift = nExpressionBlock * 2;

	const int size = m_nodes.size();
	for( int i = 0; i < size; i++ )
	{
		const Node& n = m_nodes[ i ];
		if( n.node == eNode::Func && n.id == eInternalFunc::Assign )
		{
			i++;
			const Node& n2 = m_nodes[ i ];
			if( n2.node == eNode::Var )
				usage[ n2.id ] |= (uint8_t)eVarAccess::Write << shift;
			else
				logWarning( "assign() is used with left-hand side that's not a variable." );
			continue;
		}

		if( n.node != eNode::Var )
			continue;
		usage[ n.id ] |= (uint8_t)eVarAccess::Read << shift;
	}
}