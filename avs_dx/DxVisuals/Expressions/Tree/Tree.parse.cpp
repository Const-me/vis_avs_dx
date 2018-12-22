#include "stdafx.h"
#include "Tree.h"
#include "../../Hlsl/parseId.hpp"
#include "../utils.hpp"

using namespace Expressions;

HRESULT Tree::parse( const CStringA& expr )
{
	clear();

	try
	{
		ExpressionContext ec;
		pushExpression( expr, ec, 0, expr.GetLength() );
		parseExpression( expr, 0, expr.GetLength() );
		m_lastStatement = 0;
		return S_OK;
	}
	catch( const std::exception & )
	{
		return E_FAIL;
	}
}

HRESULT Tree::appendAssignment( const CStringA& lhs, const CStringA& rhs )
{
	if( lhs.GetLength() <= 0 || rhs.GetLength() <= 0 )
	{
		logError( "Empty assignment" );
		return E_INVALIDARG;
	}

	try
	{
		ExpressionContext ec;
		const CStringA ass = "assign";
		const int indAssign = (int)m_nodes.size();

		Node node;
		node.node = eNode::Func;
#ifdef DEBUG
		node.source = "assign";
#endif
		node.id = eInternalFunc::Assign;
		pushNode( ec, std::move( node ) );

		ExpressionContext argContext;
		pushExpression( lhs, argContext, 0, lhs.GetLength() );
		parseExpression( lhs, 0, lhs.GetLength() );

		pushExpression( rhs, argContext, 0, rhs.GetLength() );
		parseExpression( rhs, 0, rhs.GetLength() );

		assert( 2 == argContext.children );
		m_nodes[ indAssign ].length = argContext.children;

		if( m_lastStatement >= 0 )
			m_nodes[ m_lastStatement ].nextSibling = indAssign;
		m_lastStatement = indAssign;

		return S_OK;
	}
	catch( const std::exception & )
	{
		return E_FAIL;
	}
}

void Tree::pushNode( ExpressionContext& ec, Node&& node )
{
	const int ind = (int)m_nodes.size();
	m_nodes.emplace_back( node );

	if( ec.prevSibling >= 0 )
		m_nodes[ ec.prevSibling ].nextSibling = ind;

	ec.prevSibling = ind;
	ec.children++;
}

void Tree::pushExpression( const CStringA& expr, ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Expr;
#ifdef DEBUG
	nn.source = expr.Mid( begin, end - begin );
#endif
	nn.length = end - begin;
	pushNode( ec, std::move( nn ) );
}

void Tree::pushCode( const CStringA& expr, ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Code;
#ifdef DEBUG
	nn.source = expr.Mid( begin, end - begin );
#endif
	nn.id = (int)m_codez.size();
	nn.length = end - begin;
	m_codez.reserve( nn.id + nn.length );
	for( int i = begin; i < end; i++ )
		m_codez.push_back( expr[ i ] );

	pushNode( ec, std::move( nn ) );
}

void Tree::pushMacro( const CStringA& expr, ExpressionContext& ec, int begin, int end )
{
	const char* const src = expr.operator const char*( ) + begin;
	const int length = end - begin;
	if( 2 == length && 0 == strncmp( src, "PI", 2 ) )
	{
		pushFloatLiteral( "3.141592653589793238", ec );
		return;
	}
	if( 1 == length && src[ 0 ] == 'E' )
	{
		pushFloatLiteral( "2.718281828459045235", ec );
		return;
	}
	if( 3 == length && 0 == strncmp( src, "PHI", 3 ) )
	{
		pushFloatLiteral( "1.618033988749894848", ec );
		return;
	}
	logError( "Unrecognized macro \"$%.*s\"", end - begin, src );
	throw std::invalid_argument( "Unrecognized macro" );
}

void Tree::pushFloatLiteral( const CStringA& val, ExpressionContext& ec )
{
	pushCode( val, ec, 0, val.GetLength() );
	m_nodes.rbegin()->vt = eVarType::f32;
}

void Tree::pushVar( const CStringA& expr, ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Var;
#ifdef DEBUG
	nn.source = expr.Mid( begin, end - begin );
#endif
	nn.id = symbols.vars.lookup( expr.Mid( begin, end - begin ), nn.vt );
	nn.length = end - begin;
	pushNode( ec, std::move( nn ) );
}

void Tree::pushFunc( const CStringA& expr, ExpressionContext& ec, int begin, int end )
{
	Node nn;
	nn.node = eNode::Func;
#ifdef DEBUG
	nn.source = expr.Mid( begin, end - begin );
#endif
	nn.id = symbols.functions.lookup( expr.Mid( begin, end - begin ), nn.vt );
	nn.length = end - begin;
	pushNode( ec, std::move( nn ) );
}

namespace
{
	inline bool isAlphaOrDollar( char c )
	{
		return isAlpha( c ) || c == '$';
	}
}

void Tree::parseExpression( const CStringA& expr, int begin, int end )
{
	using namespace Hlsl;

	const int indThis = (int)m_nodes.size() - 1;
	ExpressionContext context;
	const char* const src = expr;

	for( int i = begin; i < end; )
	{
		// Skip non-ID part, if any
		if( !isAlphaOrDollar( src[ i ] ) )
		{
			const int codeStart = i;
			for( i++; i < end && !isAlphaOrDollar( src[ i ] ); i++ ) {}
			pushCode( expr, context, codeStart, i );
			continue;
		}

		if( src[ i ] == '$' )
		{
			// Start of the dollar macro.
			i++;
			const int macroStart = i;
			for( ; i < end && isAlpha( src[ i ] ); i++ ) {}
			pushMacro( expr, context, macroStart, i );
			continue;
		}

		// Found first ID character
		const int idStart = i;
		for( i++; i < end && isAlphaNumeric( src[ i ] ); i++ ) {}
		const int idEnd = i;

		// Parsed the complete ID.
		if( idEnd >= end || src[ idEnd ] != '(' )
		{
			// It's a variable: a function call would continue with '('
			pushVar( expr, context, idStart, idEnd );
			continue;
		}

		// It's a function call.
		const size_t indFunc = m_nodes.size();
		pushFunc( expr, context, idStart, idEnd );

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
			if( argStart == i )
			{
				logError( "Empty argument of function %.*s()", idEnd - idStart, src + idStart );
				throw std::invalid_argument( "Empty function argument" );
			}
			pushExpression( expr, argContext, argStart, i );
			parseExpression( expr, argStart, i );
			if( c == ')' )
			{
				iLevel--;
				break;
			}
			argStart = i + 1;
		}

		if( 0 != iLevel )
		{
			logError( "Unmatched '(' bracket in the expression %s at %i", cstr( expr ), idEnd );
			throw std::logic_error( "Unmatched bracket" );
		}
		assert( src[ i ] == ')' );
		i++;
		m_nodes[ indFunc ].length = argContext.children;
	}

	// If this expression has the only child, erase the expression.
	if( 1 == context.children )
	{
		m_nodes.erase( m_nodes.begin() + indThis );
		// Fix nextSibling indices broken by this erase
		const int fixStart = std::max( m_lastStatement, 0 );
		const int fixEnd = (int)m_nodes.size();
		for( int i = fixStart; i < fixEnd; i++ )
		{
			int &ns = m_nodes[ i ].nextSibling;
			if( ns <= indThis )
				continue;
			ns--;
		}
	}
	else
		m_nodes[ indThis ].length = context.children;
}