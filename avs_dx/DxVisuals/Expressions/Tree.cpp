#include "stdafx.h"
#include "Tree.h"
#include "../Hlsl/parseId.hpp"
#include "utils.hpp"

using namespace Expressions;

HRESULT Tree::parse( SymbolTable& symbols, const CStringA& expr )
{
	m_nodes.clear();
	m_source = expr;

	try
	{
		ExpressionContext ec;
		pushExpression( ec, 0, expr.GetLength() );
		parseExpression( symbols );
		return S_OK;
	}
	catch( const std::exception & )
	{
		return E_FAIL;
	}
}

void Tree::pushNode( ExpressionContext& ec, Node& node )
{
#ifdef DEBUG
	node.source = m_source.Mid( node.start, node.length );
#endif

	const int ind = (int)m_nodes.size();
	m_nodes.push_back( node );

	if( ec.prevSibling >= 0 )
		m_nodes[ ec.prevSibling ].nextSibling = ind;

	ec.prevSibling = ind;
	ec.children++;
}

void Tree::pushExpression( ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Expr;
	nn.start = begin;
	nn.length = end - begin;
	pushNode( ec, nn );
}

void Tree::pushCode( ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Code;
	nn.start = begin;
	nn.length = end - begin;
	pushNode( ec, nn );
}

void Tree::pushVar( ExpressionContext& ec, SymbolTable& symbols, int begin, int end )
{
	Node nn;
	nn.node = eNode::Var;
	nn.start = begin;
	nn.length = end - begin;
	nn.id = symbols.varLookup( m_source.Mid( begin, end - begin ), nn.vt );
	pushNode( ec, nn );
}

void Tree::pushFunc( ExpressionContext& ec, SymbolTable& symbols, int begin, int end )
{
	Node nn;
	nn.node = eNode::Func;
	nn.start = begin;
	nn.length = end - begin;
	nn.id = symbols.funcLookup( m_source.Mid( begin, end - begin ), nn.vt );
	pushNode( ec, nn );
}

void Tree::parseExpression( SymbolTable& symbols )
{
	using namespace Hlsl;

	const int indThis = (int)m_nodes.size() - 1;
	const int begin = m_nodes[ indThis ].start;
	const int end = begin + m_nodes[ indThis ].length;
	ExpressionContext context;
	const char* const src = m_source;

	for( int i = begin; i < end; )
	{
		// Skip non-ID part, if any
		if( !isAlpha( src[ i ] ) )
		{
			const int codeStart = i;
			for( i++; i < end && !isAlpha( src[ i ] ); i++ ) { }
			pushCode( context, codeStart, i );
			continue;
		}

		// Found first ID character
		const int idStart = i;
		for( i++; i < end && isAlphaNumeric( src[ i ] ); i++ ) { }
		const int idEnd = i;

		// Parsed the complete ID.
		if( idEnd >= end || src[ idEnd ] != '(' )
		{
			// It's a variable: a function call would continue with '('
			pushVar( context, symbols, idStart, idEnd );
			continue;
		}

		// It's a function call.
		pushFunc( context, symbols, idStart, idEnd );

		// Parse function's arguments
		assert( src[ i ] == '(' );
		i++;
		int iLevel = 1;
		ExpressionContext argContext;
		int argStart = i;
		for( ; i < end; i++ )
		{
			const char c = src[ i ];
			if( c == '(' )
			{
				iLevel++;
				continue;
			}
			if( 1 != iLevel )
			{
				if( c == ')' )
					iLevel--;
				continue;
			}
			// OK, we're at the correct level
			if( c != ')' && c != ',' )
				continue;

			// Parsed the function's argument
			pushExpression( argContext, argStart, i );
			parseExpression( symbols );
			if( c == ')' )
			{
				iLevel--;
				break;
			}
			argStart = i + 1;
		}

		if( 0 != iLevel )
		{
			logError( "Unmatched '(' bracket in the expression %s at %i", cstr( m_source ), idEnd );
			throw std::logic_error( "Unmatched bracket" );
		}
		assert( src[ i ] == ')' );
		i++;
	}

	// If this expression has the only child, erase the expression.
	if( 1 == context.children )
	{
		m_nodes.erase( m_nodes.begin() + indThis );
		for( int i = 0; i < indThis; i++ )
		{
			int &ns = m_nodes[ i ].nextSibling;
			if( ns < indThis )
				continue;
			ns--;
		}
	}
}