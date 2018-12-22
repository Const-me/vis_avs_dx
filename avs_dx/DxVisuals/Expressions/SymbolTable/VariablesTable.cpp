#include "stdafx.h"
#include "VariablesTable.h"
#include "../utils.hpp"
using namespace Expressions;

VariablesTable::VariablesTable() = default;

VariablesTable::VariablesTable( const Prototype& proto )
{
	proto.enumVariables( [ this ]( const VariableDecl& v )
	{
		const int id = (int)table.size();
		table.push_back( v );
		map[ v.name ] = id;
	} );
}

int VariablesTable::lookup( const CStringA& name, eVarType& vt )
{
	auto p = map.Lookup( name );
	if( nullptr == p )
	{
		const int id = (int)table.size();
		table.emplace_back( VariableDecl{ eVarLocation::local, vt, name } );
		map[ name ] = id;
		return id;
	}

	VariableDecl& var = table[ p->m_value ];
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
			logWarning( "Variable %s is re-declared as different type, %s -> %s", cstr( name ), hlslName( var.vt ), hlslName( vt ) );
		break;
	}
	return p->m_value;
}


eVarType VariablesTable::type( int id ) const
{
	return table[ id ].vt;
}

eVarType VariablesTable::setType( int id, eVarType vt )
{
	auto& vtOld = table[ id ].vt;

	int flag = 0;
	if( vt != eVarType::unknown )
		flag |= 1;
	if( vtOld != eVarType::unknown )
		flag |= 2;
	switch( flag )
	{
	case 1:
		vtOld = vt;
		return vt;
	case 2:
		return vtOld;
	case 3:
		if( vtOld != vt )
			logWarning( "Variable %s is reset to another type: %s -> %s", cstr( table[ id ].name ), hlslName( vtOld ), hlslName( vt ) );
		return vtOld;
	}
	return eVarType::unknown;
}