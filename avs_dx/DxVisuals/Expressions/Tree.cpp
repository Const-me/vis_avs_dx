#include "stdafx.h"
#include "Tree.h"
#include "../Hlsl/parseId.hpp"

using namespace Expressions;

HRESULT Tree::parse( const CStringA& expr )
{
	m_nodes.clear();
	m_patches.RemoveAll();

	try
	{
		parseExpression( expr, 0, expr.GetLength() );
		return S_OK;
	}
	catch( const std::exception & )
	{
		return E_FAIL;
	}
}

void Tree::parseExpression( const CStringA& expr, int begin, int end )
{
	using namespace Hlsl;

	m_nodes.emplace_back( Node{ eNode::Expr, begin, end - begin } );
	// Expression& e = std::get<Expression>( *m_data.rbegin() );
	int indPrev = -1;

	const char* const src = expr;

	for( int i = begin; i < end; )
	{
		if( !isAlpha( src[ i ] ) )
		{
			i++;
			continue;
		}
		const int idStart = i;

		for( i++; i < end; )
		{
			if( isAlphaNumeric( src[ i ] ) )
			{
				i++;
				continue;
			}
			break;
		}
		const int idEnd = i;

		// OK, found an ID.

		if( src[ idEnd ] != '(' )
		{
			// CStrings are null-terminated, i.e. no need for extra check for the end of the string
			const int indNew = (int)m_nodes.size();
			m_nodes.emplace_back( Node{ eNode::Var, idStart, idEnd - idStart } );
			if( indPrev >= 0 )
				m_nodes[ indPrev ].nextSibling = indNew;
			indPrev = indNew;
			continue;
		}

		// It's a function call.
		m_nodes.emplace_back( Node{ eNode::Func, idStart, idEnd - idStart } );
		// Function& func = std::get<Function>( *m_data.rbegin() );

		for( ; i < end; i++ )
		{
			const char c = src[ i ];
			if( c == ')' )
				break;
		}

	}

	throw std::logic_error( "E_NOTIMPL" );
}

bool Tree::transformRandom()
{
	bool res = false;
	for( auto& n : m_nodes )
	{
		if( n.node != eNode::Func )
			continue;
		const CStringA id = m_source.Mid( n.start, n.length );
		// if()
	}
	return res;
}