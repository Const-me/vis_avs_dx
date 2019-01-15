#include "stdafx.h"
#include "VariablesTable.h"
#include "../utils.hpp"
#include "isHlslKeyword.h"
using namespace Expressions;

VariablesTable::VariablesTable() = default;

VariablesTable::VariablesTable( const Prototype& proto )
{
	proto.enumVariables( [ this, &proto ]( const VariableDecl& v )
	{
		const int id = (int)table.size();
		table.push_back( v );
		map[ v.name ] = id;
		if( v.name == proto.getBeatMacro() )
			m_beatMacro = id;
	} );
	prototypeSize = (int)table.size();
}

int VariablesTable::addNewVariable( const CStringA& name, eVarType& vt )
{
	assert( nullptr == map.Lookup( name ) );
	const int id = (int)table.size();

	if( isHlslKeyword( name ) )
		table.emplace_back( VariableDecl{ eVarLocation::unknown, vt, "_var_" + name } );
	else
		table.emplace_back( VariableDecl{ eVarLocation::unknown, vt, name } );

	map[ name ] = id;
	return id;
}

int VariablesTable::lookup( const CStringA& name, eVarType& vt )
{
	auto p = map.Lookup( name );
	if( nullptr == p )
		return addNewVariable( name, vt );

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

void VariablesTable::clearLocals()
{
	table.resize( prototypeSize );
	map.RemoveAll();
	for( int i = 0; i < prototypeSize; i++ )
		map[ table[ i ].name ] = i;
}

void VariablesTable::defaultTypesToFloat()
{
	for( auto& v : table )
	{
		if( v.vt != eVarType::unknown )
			continue;
		v.vt = eVarType::f32;
	}
}