#include "stdafx.h"
#include "SuperScope.h"

using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addConstantInput( "w", eVarType::u32 );	// screen width in pixels
			addConstantInput( "h", eVarType::u32 );	// screen height in pixels
			addConstantInput( "drawingLines", eVarType::u32 );

			addState( "n", 768u );

			addFragmentOutput( "x" );
			addFragmentOutput( "y" );

			addFragmentOutput( "skip" );
			addFragmentOutput( "linesize" );

			addFragmentOutput( "red" );
			addFragmentOutput( "green" );
			addFragmentOutput( "blue" );

			addFragmentInput( "i" );
			addFragmentInput( "v" );

			addIndirectDrawArgs( "drawArgs", uint4{ 0, 1, 0, 0 }, "drawArgs.x = drawingLines ? n + 2 : n;" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

ScopeBase::DynamicStateData::DynamicStateData() :
	Expressions::Compiler( "SuperScope", prototype(), FixedStateData::stateSize() ) { }

HRESULT ScopeBase::FixedStateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

HRESULT ScopeBase::StateData::update( AvsState& avs )
{
	BoolHr res = m_fixed.update( avs );
	res.combine( m_dynamic.update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() ) );
	return res;
}

UINT ScopeBase::StateData::stateSize() const
{
	return FixedStateData::stateSize() + m_dynamic.stateSize();
}

ScopeBase::StateData::StateData( const AvsState& s ) :
	m_fixed( s ),
	m_template( "SuperScope" )
{ }

const StateShaderTemplate* ScopeBase::StateData::shaderTemplate()
{
	// Merging two state shaders: one came from SuperScopeState.hlsl to cycle colors, another one from NSEEL compiler.
	const StateShaderTemplate& tFixed = *m_fixed.shaderTemplate();
	const StateShaderTemplate& tDynamic = *m_dynamic.shaderTemplate();

	if( nullptr != tFixed.globals )
		m_templateGlobals = *tFixed.globals;
	else
		m_templateGlobals.clear();

	if( nullptr != tDynamic.globals )
		for( const CStringA& s : *tDynamic.globals )
			m_templateGlobals.push_back( s );
	
	m_template.globals = &m_templateGlobals;

	m_template.hlslMain = tFixed.hlslMain;
	m_template.hlslMain += "\r\n";
	m_template.hlslMain += tDynamic.hlslMain;

	m_template.hasBeat = tFixed.hasBeat || tDynamic.hasBeat;
	m_template.hasRandomNumbers = tDynamic.hasRandomNumbers;

	return &m_template;
}