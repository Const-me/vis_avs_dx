#include "stdafx.h"
#include "Bump.h"

IMPLEMENT_EFFECT( Bump, C_BumpClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "isbeat" );
			addState( "x", 0.5f );
			addState( "y", 0.5f );
			addState( "bi", 1.0f );
			addState( "islbeat", (uint32_t)0, true );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

BumpStructs::DynamicStateData::DynamicStateData() :
	Expressions::Compiler( "Bump", prototype(), FixedStateData::stateSize() ) { }

HRESULT BumpStructs::FixedStateData::update( const AvsState& ass )
{
	BoolHr hr;
	hr.updateValue( bumpNormal, ass.depth * div255 );
	hr.updateValue( bumpOnBeat, ass.depth2 * div255 );
	hr.updateValue( onBeat, ass.onbeat ? true : false );
	hr.updateValue( durationFrames, ass.durFrames );
	return hr;
}

HRESULT BumpStructs::StateData::update( AvsState& avs )
{
	BoolHr res = m_fixed.update( avs );
	res.combine( m_dynamic.update( avs.code3.get(), avs.code1.get(), avs.code2.get(), "" ) );
	return res;
}

UINT BumpStructs::StateData::stateSize() const
{
	return FixedStateData::stateSize() + m_dynamic.stateSize();
}

BumpStructs::StateData::StateData( const AvsState& s ) :
	m_fixed( s ),
	m_template( "SuperScope" )
{ }

const StateShaderTemplate* BumpStructs::StateData::shaderTemplate()
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

HRESULT BumpStructs::StateData::defines( Hlsl::Defines& def ) const
{
	CHECK( m_fixed.defines( def ) );
	CHECK( m_dynamic.defines( def ) );
	return S_OK;
}

Bump::Bump( AvsState *pState ) :
	EffectBase1( pState )
{ }


HRESULT Bump::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	omClearTargets();

	BoundPsResource boundDepth, boundColor;
	if( avs->buffern > 0 )
	{
		RenderTargets& rtDepth = StaticResources::globalBuffers[ avs->buffern - 1 ];
		boundDepth = rtDepth.lastWritten().psView( renderer.pixel().depthTexture );
	}
	else
		boundDepth = rt.lastWritten().psView( renderer.pixel().depthTexture );

	CHECK( rt.writeToNext( boundColor ) );

	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}