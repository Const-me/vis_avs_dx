#include "stdafx.h"
#include "SymbolTable.h"
#include "utils.hpp"
#include "builtinFunctions.h"
#include "includeFunctions.h"

using namespace Expressions;

SymbolTable::SymbolTable()
{
	addInternalFunc( "assign" );
	addInternalFunc( "equal" );
	addInternalFunc( "if" );
}

int SymbolTable::addFunc( const CStringA& name, eFunctionKind kind, eVarType vt )
{
	assert( nullptr == functionsMap.Lookup( name ) );
	const int id = (int)functions.size();
	functions.emplace_back( Function{ name, kind, vt } );
	functionsMap[ name ] = id;
	return id;
}

int SymbolTable::addInternalFunc( const CStringA& name )
{
	return addFunc( name, eFunctionKind::Internal, eVarType::unknown );
}

int SymbolTable::varLookup( const CStringA& name, eVarType& vt )
{
	auto p = variablesMap.Lookup( name );
	if( nullptr == p )
	{
		const int id = (int)variables.size();
		variables.emplace_back( Variable{ name, vt } );
		variablesMap[ name ] = id;
		return id;
	}

	Variable& var = variables[ p->m_value ];
	int flag = 0;
	if( vt != eVarType::unknown )
		flag |= 1;
	if( var.vt != eVarType::unknown )
		flag |= 2;
	switch( flag )
	{
	case 1:
		var.vt = vt;
		break;
	case 2:
		vt = var.vt;
		break;
	case 3:
		if( var.vt != vt )
			logWarning( "Variable %s is re-declared as different type", cstr( name ) );
		break;
	}
	return p->m_value;
}

int SymbolTable::funcLookup( const CStringA& name, eVarType &vt )
{
	auto p = functionsMap.Lookup( name );
	if( nullptr != p )
	{
		const int id = p->m_value;
		const Function& func = functions[ id ];
		vt = func.vt;
		return id;
	}

	auto avs = lookupShaderFunc( name );
	if( nullptr != avs )
	{
		vt = avs->returnType;
		return addFunc( name, eFunctionKind::Avs, vt );
	}

	if( hasDoubleVersion( name, vt ) )
	{
		// sin/cos/tan are polymorphic however they have same return type as the built-in, i.e. it's known already
		if( vt != eVarType::f32 )
			vt = eVarType::unknown;
		return addFunc( name, eFunctionKind::Polymorphic, vt );
	}

	if( isBuiltinFunction( name, vt ) )
	{
		return addFunc( name, eFunctionKind::Hlsl, vt );
	}

	vt = eVarType::unknown;
	return addFunc( name, eFunctionKind::unknown, vt );
}