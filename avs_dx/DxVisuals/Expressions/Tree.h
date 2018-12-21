#pragma once
#include "SymbolTable.h"

namespace Expressions
{
	enum struct eNode : uint8_t
	{
		// A sub-expression
		Expr,
		// Variable
		Var,
		// Function invocation
		Func,
		// Everything else, e.g. "(" or ")+14"
		Code,
	};

	struct Node
	{
		eNode node;
		eVarType vt = eVarType::unknown;

#ifdef DEBUG
		CStringA source;
		int start, id;
#else
		union
		{
			// Used for sub-expressions and for code
			int start;
			// For variables and functions, this is the ID from SymbolTable.
			int id;
		};
#endif
		int length;

		// The parser emits nodes in depth-first order. This field holds index in m_nodes of the next sibling element, or -1 of this is the final node of the sub-tree.
		int nextSibling = -1;
	};

	class Tree
	{
		CStringA m_source;
		std::vector<Node> m_nodes;

		struct ExpressionContext
		{
			int prevSibling = -1;
			int children = 0;
		};

		void parseExpression( SymbolTable& symbols );

		void pushNode( ExpressionContext& ec, Node& node );
		void pushCode( ExpressionContext& ec, int begin, int end );
		void pushVar( ExpressionContext& ec, SymbolTable& symbols, int begin, int end );
		void pushFunc( ExpressionContext& ec, SymbolTable& symbols, int begin, int end );

		void pushExpression( ExpressionContext& ec, int begin, int end );

	public:

		HRESULT parse( SymbolTable& symbols, const CStringA& expr );

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