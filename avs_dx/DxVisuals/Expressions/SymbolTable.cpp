#include "stdafx.h"
#include "SymbolTable.h"
#include "utils.hpp"
#include "builtinFunctions.h"
#include "includeFunctions.h"

using namespace Expressions;

SymbolTable::SymbolTable()
{
	addInternalFunc( "equal" );
	addInternalFunc( "if" );
}

void SymbolTable::addInternalFunc( const CStringA& name )
{
	const int id = (int)names.size();
	names.push_back( name );
	Function func;
	func.id = id;
	func.kind = SymbolTable::Internal;
	functions[ name ] = func;
}

int SymbolTable::varLookup( const CStringA& name, eVarType& vt )
{
	auto p = variables.Lookup( name );
	if( nullptr == p )
	{
		const int id = (int)names.size();
		names.push_back( name );
		Variable var;
		var.id = id;
		var.vt = vt;
		variables[ name ] = var;
		return id;
	}

	int flag = 0;
	if( vt != eVarType::unknown )
		flag |= 1;
	if( p->m_value.vt != eVarType::unknown )
		flag |= 2;
	switch( flag )
	{
	case 1:
		p->m_value.vt = vt;
		break;
	case 2:
		vt = p->m_value.vt;
		break;
	case 3:
		if( p->m_value.vt != vt )
			logWarning( "Variable %s is re-declared as different type", cstr( name ) );
		break;
	}
	return p->m_value.id;
}

int SymbolTable::funcLookup( const CStringA& name, eVarType &vt )
{
	auto p = functions.Lookup( name );
	if( nullptr != p )
	{
		vt = p->m_value.vt;
		return p->m_value.id;
	}

	const int id = (int)names.size();
	names.push_back( name );
	Function func;
	func.id = id;

	auto avs = lookupShaderFunc( name );
	if( nullptr != avs )
	{
		func.kind = eFunctionKind::Avs;
		vt = func.vt = avs->returnType;
		functions[ name ] = func;
		return id;
	}

	if( hasDoubleVersion( name, vt ) )
	{
		func.kind = eFunctionKind::Polymorphic;
		// sin/cos/tan are polymorphic however they have same return type as the built-in, i.e. it's known already
		if( vt != eVarType::f32 )
			vt = eVarType::unknown;
		func.vt = vt;
		functions[ name ] = func;
		return id;
	}

	if( isBuiltinFunction( name, vt ) )
	{
		func.kind = eFunctionKind::Hlsl;
		func.vt = vt;
		functions[ name ] = func;
		return id;
	}

	functions[ name ] = func;
	vt = eVarType::unknown;
	return id;
}