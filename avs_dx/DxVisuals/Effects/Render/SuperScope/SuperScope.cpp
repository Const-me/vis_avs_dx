#include "stdafx.h"
#include "SuperScope.h"

IMPLEMENT_EFFECT( SuperScope, C_SScopeClass );

bool ScopeBase::AvsState::drawingLines() const
{
	return mode > 0;
}

namespace
{
	eSource source( int which_ch )
	{
		return ( which_ch & 4 ) ? eSource::Spectrum : eSource::Wave;
	}

	eChannel channel( int which_ch )
	{
		which_ch &= 3;
		if( 0 == which_ch ) return eChannel::Left;
		if( 1 == which_ch ) return eChannel::Right;
		return eChannel::Center;
	}
}

float ScopeBase::AvsState::sampleV() const
{
	return sourceSampleV( source( which_ch ), channel( which_ch ) );
}

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
	res.updateValue( screenSize, getRenderSize() );
	res.updateValue( drawingLines, avs.drawingLines() );
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

HRESULT ScopeBase::StateData::defines( Hlsl::Defines& def ) const
{
	CHECK( m_fixed.defines( def ) );
	CHECK( m_dynamic.defines( def ) );
	def.set( "drawingLines", drawingLines ? "1" : "0" );
	setExpressionMacro( def, "w", screenSize.cx );
	setExpressionMacro( def, "h", screenSize.cy );
	return S_OK;
}

HRESULT ScopeBase::VsData::defines( Hlsl::Defines& def )
{
	CHECK( __super::defines( def ) );
	const CSize screenSize = getRenderSize();
	setExpressionMacro( def, "w", screenSize.cx );
	setExpressionMacro( def, "h", screenSize.cy );
	return S_OK;
}

HRESULT ScopeBase::VsData::updateAvs( const AvsState& avs )
{
	BoolHr res;
	res.updateValue( sampleV, avs.sampleV() );
	res.updateValue<uint32_t>( drawLines, avs.drawingLines() );
	return res;
}

HRESULT SuperScope::render( bool isBeat, RenderTargets& rt )
{
	// omBlend( eBlend::Premultiplied );
	StaticResources::globalBlendModes.setupBlending();

	CHECK( rt.writeToLast( false ) );
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	iaClearBuffer();

	const UINT argsOffset = ( stateOffset() + stateData.compiler().getIndirectArgOffset() ) * 4;

	if( avs->drawingLines() )
	{
		if( !eastl::holds_alternative<LinesRendering>( m_render ) )
			m_render.emplace<LinesRendering>();

		LinesRendering& lines = eastl::get<LinesRendering>( m_render );

		if( !lines.gs.hasShader() )
			CHECK( lines.gs.compile( Hlsl::includes(), 0 ) );
		if( !lines.ps.hasShader() )
			CHECK( lines.ps.compile( Hlsl::includes(), 0 ) );
		if( !lines.gs.hasShader() || !lines.ps.hasShader() )
			return S_FALSE;

		bindShader<eStage::Geometry>( lines.gs.ptr( false ) );
		bindShader<eStage::Pixel>( lines.ps.ptr( false ) );
		context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
		context->DrawInstancedIndirect( stateBuffer(), argsOffset );
	}
	else
	{
		if( !eastl::holds_alternative<DotRendering>( m_render ) )
			m_render.emplace<DotRendering>();

		DotRendering& dots = eastl::get<DotRendering>( m_render );

		if( !dots.gs.hasShader() )
			CHECK( dots.gs.compile( Hlsl::includes(), 0 ) );
		if( !dots.ps.hasShader() )
			CHECK( dots.ps.compile( Hlsl::includes(), 0 ) );
		if( !dots.gs.hasShader() || !dots.ps.hasShader() )
			return S_FALSE;

		bindShader<eStage::Geometry>( dots.gs.ptr( false ) );
		bindShader<eStage::Pixel>( dots.ps.ptr( false ) );
		context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
		context->DrawInstancedIndirect( stateBuffer(), argsOffset );
	}
	return S_OK;
}