#include "stdafx.h"
#include "Prototype.h"
#include "utils.hpp"

using namespace Expressions;

HRESULT Prototype::addState( const CStringA& name, uint32_t def )
{
	CStringA str;
	str.Format( "%i", def );
	return addState( name, eVarType::u32, str );
}

HRESULT Prototype::addState( const CStringA& name, float def )
{
	CStringA str;
	str.Format( "%g", def );
	return addState( name, eVarType::f32, str );
}

HRESULT Prototype::addState( const CStringA& name, eVarType vt, const CStringA& initVal )
{
	for( const auto& s : m_fixedState )
		if( s.name == name )
			return E_INVALIDARG;

	m_fixedState.emplace_back( sFixedStateVar{} );
	sFixedStateVar& v = *m_fixedState.rbegin();
	v.name = name;
	v.initVal = initVal;
	v.vt = vt;
	v.offset = m_size;
	m_size++;
	return S_OK;
}

HRESULT Prototype::addInputOutput( const CStringA& name, eVarType vt, std::vector<sInputOutput>& vec )
{
	for( const auto& s : vec )
		if( s.name == name )
			return E_INVALIDARG;
	vec.emplace_back( sInputOutput{ name,vt } );
	return S_OK;
}

CStringA Prototype::initState() const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
		hlsl.AppendFormat( "		%s %s = %s;\r\n", hlslName( s.vt ), cstr( s.name ), cstr( s.initVal ) );
	return hlsl;
}

CStringA Prototype::stateLoad() const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
	{
		const CStringA load = Expressions::stateLoad( s.vt, s.offset );
		hlsl.AppendFormat( "		%s %s = %s;\r\n", hlslName( s.vt ), cstr( s.name ), cstr( load ) );
	}
	return hlsl;
}

CStringA Prototype::stateStore() const
{
	CStringA hlsl;
	for( const auto& s : m_fixedState )
	{
		const CStringA store = Expressions::stateStore( s.vt, s.offset, s.name );
		hlsl.AppendFormat( "		%s;\r\n", cstr( store ) );
	}
	return hlsl;
}