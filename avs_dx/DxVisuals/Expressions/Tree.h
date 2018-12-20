#pragma once
#include <atlcoll.h>
#include "eVarType.h"

namespace Expressions
{
	enum struct eNode : uint8_t
	{
		// A sub-expression
		Expr,
		// an identifier that's variable or maybe a macro
		Var,
		// an identifier that's a function invocation
		Func,
	};

	struct Node
	{
		eNode node;
		eVarType vt = eVarType::unknown;
		bool hasChildren = false;

		// For expressions, that's the complete string. For variables and functions, just the name part.
		int start, length;
		// The parser emits nodes in depth-first order. This field holds index in m_nodes of the next sibling element, or -1 of this is the final node of the sub-tree.
		int nextSibling = -1;

		Node( eNode e, int s, int l ) : node( e ), start( s ), length( l ) { }
	};

	class Tree
	{
		CStringA m_source;
		std::vector<Node> m_nodes;

		struct sPatch
		{
			int removeLength;
			CStringA insert;
		};

		CRBMap<int, sPatch> m_patches;

		void parseExpression( const CStringA& expr, int begin, int end );

	public:

		HRESULT parse( const CStringA& expr );

		// Patch rand( xx ) -> rand( rng_state, xx ). Return false if the expression doesn't use random number generation.
		bool transformRandom();

		// HRESULT deduceTypes

		template<class Func>
		void visitVariables( Func&& fn )
		{
			for( auto& n : m_nodes )
			{
				if( n.node != eNode::Var )
					continue;
				const CStringA id = m_source.Mid( n.start, n.length );
				fn( n, id );
			}
		}
	};
}