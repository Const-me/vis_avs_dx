#include "stdafx.h"
#include "Tree.h"
using namespace Expressions;

struct Tree::EmitContext
{
	CStringA& result;
	bool haveRandom = false;

	EmitContext( CStringA& hlsl ) :
		result( hlsl )
	{
		result = "";
		result.Preallocate( 1024 );
	}

	void lineBegin()
	{
		for( int i = 0; i < indent; i++ )
			result += '\t';
	}
	void lineEnd()
	{
		result += ";\r\n";
	}

	void code( const Node& node, const vector<char> &codez )
	{
		assert( node.node == eNode::Code );
		result.AppendFormat( "%.*s", node.length, codez.data() + node.id );
	}
	void operator+=( const CStringA& str )
	{
		result += str;
	}
private:
	int indent = 1;
};

void Tree::emitNode( EmitContext& ec, int ind ) const
{
	const Node& node = m_nodes[ ind ];
	const eNode nt = node.node;

	switch( nt )
	{
	case eNode::Code:
		ec.code( node, m_codez );
		return;
	case eNode::Var:
		ec += symbols.vars.name( node.id );
		return;
	case eNode::Expr:
		for( int i = ind + 1; true; )
		{
			emitNode( ec, i );
			if( !nextSibling( i ) )
				break;
		}
		return;
	}

	emitFunction( ec, ind );
}

template<>
void Tree::emitInternal<eInternalFunc::Assign>( EmitContext& ec, const Node& node, int ind ) const
{
	if( node.length != 2 )
		throw std::invalid_argument( "assign() must have exactly 2 arguments" );
	int i = ind + 1;
	emitNode( ec, i );
	ec += " = ";
	nextSibling( i );
	emitNode( ec, i );
}

template<>
void Tree::emitInternal<eInternalFunc::Equals>( EmitContext& ec, const Node& node, int ind ) const
{
	if( node.length != 2 )
		throw std::invalid_argument( "equals() must have exactly 2 arguments" );
	int i = ind + 1;
	emitNode( ec, i );
	ec += " == ";
	nextSibling( i );
	emitNode( ec, i );
}

template<>
void Tree::emitInternal<eInternalFunc::If>( EmitContext& ec, const Node& node, int ind ) const
{
	if( node.length != 3 )
		throw std::invalid_argument( "if() must have exactly 3 arguments" );
	ec += "( ";
	int i = ind + 1;
	emitNode( ec, i );
	ec += " ) ? ";
	nextSibling( i );
	emitNode( ec, i );
	ec += " : ";
	nextSibling( i );
	emitNode( ec, i );
}

template<>
void Tree::emitInternal<eInternalFunc::Rand>( EmitContext& ec, const Node& node, int ind ) const
{
	if( node.length != 1 )
		throw std::invalid_argument( "rand() must have exactly 1 argument" );
	ec += "avs_rand( rng_state, ";
	emitNode( ec, ind + 1 );
	ec += " )";
	ec.haveRandom = true;
	// Include the relevant functions into the symbols table, so they're included in the global sections of the output shader.
	symbols.functions.addAvs( "avs_rand_init" );
	symbols.functions.addAvs( "avs_rand" );
}

// A bit of C++ magic here: instantiate the emitters for internal functions.
// They're specialized templates but the following code instantiates them, and places pointers to these functions into the static array.
const array<Tree::pfnEmitNode, 4> Tree::s_emitInternal =
{
	&Tree::emitInternal<0>,
	&Tree::emitInternal<1>,
	&Tree::emitInternal<2>,
	&Tree::emitInternal<3>,
};

void Tree::emitFunction( EmitContext& ec, int ind ) const
{
	const Node& node = m_nodes[ ind ];
	assert( node.node == eNode::Func );

	if( node.id < (int)s_emitInternal.size() )
	{
		( this->*s_emitInternal[ node.id ] )( ec, node, ind );
		return;
	}

	ec += symbols.functions.name( node.id );
	if( 0 == node.length )
	{
		ec += "()";
		return;
	}

	ec += "( ";
	for( int i = ind + 1; true; )
	{
		if( i != ind + 1 )
			ec += ", ";
		emitNode( ec, i );
		if( !nextSibling( i ) )
			break;
	}
	ec += " )";
}

HRESULT Tree::emitHlsl( CStringA& hlsl, bool& usesRng ) const
{
	if( m_nodes.empty() )
	{
		hlsl = "";
		return S_FALSE;
	}

	try
	{
		EmitContext ec{ hlsl };
		for( int i = 0; i >= 0; )
		{
			// TODO: search for the stupid trick below, and emit top-level "if" instead of nested-level `operator ?`
			// From util.cpp:265
			// a little trick: assign(if(v,a,b),1.0); is like if V is true, a=1.0, otherwise b=1.0. :)

			ec.lineBegin();
			emitNode( ec, i );
			ec.lineEnd();
			i = m_nodes[ i ].nextSibling;
		}
		usesRng = usesRng || ec.haveRandom;
		return S_OK;
	}
	catch( const std::exception& ex )
	{
		logError( "emitHlsl failed: %s", ex.what() );
		return E_FAIL;
	}
}