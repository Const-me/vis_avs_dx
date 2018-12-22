#include "stdafx.h"
#include "FunctionsTable.h"
#include "../Builtin/builtinFunctions.h"
#include "../Builtin/includeFunctions.h"

using namespace Expressions;

FunctionsTable::FunctionsTable()
{
	addInternals();
}

void FunctionsTable::addInternals()
{
	int i = add( "assign", eFunctionKind::Internal );
	assert( i == eInternalFunc::Assign );

	i = add( "equal", eFunctionKind::Internal, eVarType::u32 );
	assert( i == eInternalFunc::Equals );

	i = add( "if", eFunctionKind::Internal );
	assert( i == eInternalFunc::If );

	i = add( "rand", eFunctionKind::Internal, eVarType::f32 );
	assert( i == eInternalFunc::Rand );
}

int FunctionsTable::add( const CStringA& name, eFunctionKind kind, eVarType vt )
{
	assert( nullptr == map.Lookup( name ) );
	const int id = (int)table.size();
	table.emplace_back( Function{ kind, vt, name } );
	map[ name ] = id;
	return id;
}

int FunctionsTable::lookup( const CStringA& name, eVarType &vt )
{
	auto p = map.Lookup( name );
	if( nullptr != p )
	{
		const int id = p->m_value;
		const Function& func = table[ id ];
		vt = func.vt;
		return id;
	}

	auto avs = lookupShaderFunc( name );
	if( nullptr != avs )
	{
		vt = avs->returnType;
		return add( name, eFunctionKind::Avs, vt );
	}

	if( hasDoubleVersion( name, vt ) )
	{
		if( vt != eVarType::f32 )
		{
			// abs/floor/min/max, the double-receiving functions return doubles, built-in float versions return floats, that's why unknowns
			vt = eVarType::unknown;
		}
		return add( name, eFunctionKind::Polymorphic, vt );
	}

	if( isBuiltinFunction( name, vt ) )
	{
		return add( name, eFunctionKind::Hlsl, vt );
	}

	vt = eVarType::unknown;
	return add( name, eFunctionKind::unknown, vt );
}

FunctionType FunctionsTable::type( int id ) const
{
	return table[ id ];
}

const CStringA& FunctionsTable::name( int id ) const
{
	return table[ id ].name;
}