#pragma once
#include "SymbolTable.h"
#include "parse.h"

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
#endif
		// For variables and functions, the ID from SymbolTable. For codes, offset in m_codez.
		int id = -1;
		// For codes, length of the data in m_codez. For expressions, children count. For functions, arguments count.
		int length = 0;

		// The parser emits nodes in depth-first order. This field holds index in m_nodes of the next sibling element, or -1 of this is the final node of the sub-tree.
		int nextSibling = -1;
	};

	class Tree
	{
		SymbolTable& symbols;
		int m_lastStatement = -1;
		std::vector<char> m_codez;
		std::vector<Node> m_nodes;

		struct ExpressionContext
		{
			int prevSibling = -1;
			int children = 0;
		};

		void pushNode( ExpressionContext& ec, Node&& node );

		void pushCode( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushVar( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushFunc( const CStringA& expr, ExpressionContext& ec, int begin, int end );
		void pushExpression( const CStringA& expr, ExpressionContext& ec, int begin, int end );

		void parseExpression( const CStringA& expr, int begin, int end );

		eVarType nodeType( int indNode );
		eVarType expressionType( int iFirstChild );
		eVarType functionType( int iFunc );

	public:

		Tree( SymbolTable& symbolsTable );

		void clear();

		HRESULT parse( const CStringA& expr );

		HRESULT appendAssignment( const CStringA& lhs, const CStringA& rhs );

		HRESULT deduceTypes();

		/* template<class Func>
		void visitVariables( Func&& fn )
		{
			for( auto& n : m_nodes )
			{
				if( n.node != eNode::Var )
					continue;
				const CStringA id = m_source.Mid( n.start, n.length );
				fn( n, id );
			}
		} */
	};
}