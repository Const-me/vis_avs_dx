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

HRESULT Tree::deduceTypes()
{
	if( m_nodes.empty() )
		return S_FALSE;

	try
	{
		for( int i = 0; i >= 0; )
		{
			nodeType( i );
			i = m_nodes[ i ].nextSibling;
		}
		return S_OK;
	}
	catch( const std::exception & )
	{
		return E_FAIL;
	}
}

eVarType Tree::nodeType( int indNode )
{
	Node& n = m_nodes[ indNode ];
	if( n.vt != eVarType::unknown )
		return n.vt;

	if( eNode::Code == n.node )
		return eVarType::unknown;

	if( eNode::Var == n.node )
	{
		n.vt = symbols.varGetType( n.id );
		return n.vt;
	}
	if( eNode::Expr == n.node )
	{
		n.vt = expressionType( indNode + 1 );
		return n.vt;
	}
	if( eNode::Func == n.node )
	{
		n.vt = functionType( indNode );
		return n.vt;
	}
	__debugbreak();
	return eVarType::unknown;
}

namespace
{
	constexpr uint32_t typeBit( eVarType vt )
	{
		return (uint32_t)1 << (uint32_t)( (uint8_t)vt );
	}

	eVarType combineTypes( uint32_t mask )
	{
		mask &= ~typeBit( eVarType::unknown );
		if( 0 == mask )
			return eVarType::unknown;

		if( 0 != ( mask & typeBit( eVarType::f64 ) ) )
			return eVarType::f64;
		if( 0 != ( mask & typeBit( eVarType::f32 ) ) )
			return eVarType::f32;
		if( 0 != ( mask & typeBit( eVarType::i32 ) ) )
			return eVarType::i32;
		return eVarType::u32;
	}
}

eVarType Tree::expressionType( int iFirstChild )
{
	static_assert( (int)eVarType::count <= 32 );
	uint32_t mask = 0;
	for( ; iFirstChild >= 0; )
	{
		mask |= typeBit( nodeType( iFirstChild ) );
		iFirstChild = m_nodes[ iFirstChild ].nextSibling;
	}
	return combineTypes( mask );
}

eVarType Tree::functionType( int iFunc )
{
	const int id = m_nodes[ iFunc ].id;

	const auto ft = symbols.funGetType( id );

	if( ft.kind == eFunctionKind::Internal )
	{
		if( id == SymbolTable::idAssign )
		{
			const int indLhs = iFunc + 1;
			const int indRhs = m_nodes[ indLhs ].nextSibling;

			const auto vtRight = nodeType( indRhs );
			if( m_nodes[ indLhs ].node == eNode::Var )
			{
				symbols.varSetType( m_nodes[ indLhs ].id, vtRight );
				m_nodes[ indLhs ].vt = vtRight;
			}
			return vtRight;
		}
		if( id == SymbolTable::idEquals )
			return eVarType::u32;

		if( id == SymbolTable::idIf )
		{
			int i = iFunc + 1;	// Ignore first argument, it's condition
			i = m_nodes[ i ].nextSibling;

			uint32_t mask = typeBit( nodeType( i ) );
			i = m_nodes[ i ].nextSibling;
			mask |= typeBit( nodeType( i ) );
			return combineTypes( mask );
		}
	}

	if( ft.kind != eFunctionKind::Polymorphic || ft.kind != eFunctionKind::unknown )
		return ft.vt;

	uint32_t mask = 0;
	for( int i = iFunc + 1; i >= 0; )
	{
		mask |= typeBit( nodeType( i ) );
		i = m_nodes[ i ].nextSibling;
	}
	return combineTypes( mask );
}