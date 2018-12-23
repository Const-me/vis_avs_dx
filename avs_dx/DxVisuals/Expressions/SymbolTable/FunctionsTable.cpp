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

int FunctionsTable::addAvs( const CStringA& name )
{
	eVarType vt;
	return addAvs( name, vt );
}

int FunctionsTable::addAvs( const CStringA& name, eVarType &vt )
{
	auto p = map.Lookup( name );
	if( nullptr != p )
	{
		const int id = p->m_value;
		vt = table[ id ].vt;
		return id;
	}

	auto avs = lookupShaderFunc( name );
	assert( nullptr != avs );
	return add( name, eFunctionKind::Avs, avs->returnType );
}

bool FunctionsTable::tryLookup( const CStringA& name, int& id, eVarType &vt )
{
	// Search in this table
	auto p = map.Lookup( name );
	if( nullptr != p )
	{
		id = p->m_value;
		vt = table[ id ].vt;
		return true;
	}

	// Search for built-in function in ../Builtin/*.hlsl
	// These sources are parsed in compile time, and the hash map is initialized in static constructors i.e. when DllMain is called with DLL_PROCESS_ATTACH
	auto avs = lookupShaderFunc( name );
	if( nullptr != avs )
	{
		vt = avs->returnType;
		id = add( name, eFunctionKind::Avs, vt );
		return true;
	}

	// Check for polymorphic function
	if( hasDoubleVersion( name, vt ) )
	{
		if( vt != eVarType::f32 )
		{
			// abs/floor/min/max, the double-receiving functions return doubles, built-in float versions return floats, that's why unknowns
			vt = eVarType::unknown;
		}
		id = add( name, eFunctionKind::Polymorphic, vt );
		return true;
	}

	// Search for HLSL built-in func like atan2 or log10
	if( isBuiltinFunction( name, vt ) )
	{
		id = add( name, eFunctionKind::Hlsl, vt );
		return true;
	}

	// Failed: user has typed garbage instead of a function.
	id = -1;
	return false;
}

FunctionType FunctionsTable::type( int id ) const
{
	return table[ id ];
}

const CStringA& FunctionsTable::name( int id ) const
{
	return table[ id ].name;
}

template<class TFunc>
void FunctionsTable::enumAvsFuncs( TFunc callback ) const
{
	for( const auto& fn : table )
	{
		if( fn.kind != eFunctionKind::Avs )
			continue;
		const ShaderFunc* impl = lookupShaderFunc( fn.name );
		assert( nullptr != impl );
		callback( impl->hlsl );
	}
}

void FunctionsTable::getStateGlobals( std::vector<CStringA>& list ) const
{
	list.clear();
	enumAvsFuncs( [ & ]( const char* hlsl )
	{
		list.push_back( hlsl );
	} );
}

CStringA FunctionsTable::getFragmentGlobals() const
{
	CStringA res;
	res.Preallocate( 1024 );
	enumAvsFuncs( [ & ]( const char* hlsl )
	{
		res += hlsl;
		res += "\r\n";
	} );
	return res;
}

void FunctionsTable::clear()
{
	const int len = eInternalFunc::valuesCount;
	table.resize( len );
	map.RemoveAll();
	for( int i = 0; i < len; i++ )
		map[ table[ i ].name ] = i;
}