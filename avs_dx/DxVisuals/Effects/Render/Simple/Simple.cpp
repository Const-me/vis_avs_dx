#include "stdafx.h"
#include "Simple.h"

// ==== Common stuff for all styles ====

IMPLEMENT_EFFECT( Simple, C_SimpleClass )
const EffectBase::Metadata& SimpleDots::metadata() { return s_metadada; }
const EffectBase::Metadata& SimpleSolid::metadata() { return s_metadada; }

namespace
{
	eSource source( int effect )
	{
		return ( effect & 2 ) ? eSource::Wave : eSource::Spectrum;
	}

	eChannel channel( int effect )
	{
		const int which_ch = ( effect >> 2 ) & 3;
		if( 0 == which_ch ) return eChannel::Left;
		if( 1 == which_ch ) return eChannel::Right;
		return eChannel::Center;
	}

	eSimpleRenderStyle getRenderStyle( int effect )
	{
		if( effect & ( 1 << 6 ) )
			return eSimpleRenderStyle::Dots;
		switch( effect & 3 )
		{
		case 0:
		case 3:
			return eSimpleRenderStyle::Solid;
		}
		return eSimpleRenderStyle::Lines;
	}
}

SimpleBase::ChildStateData::ChildStateData( const SimpleBase::AvsState& avs ) :
	renderStyle( getRenderStyle( avs.effect ) )
{ }

float SimpleBase::AvsState::sampleV() const
{
	return sourceSampleV( source( effect ), channel( effect ) );
}

HRESULT SimpleBase::StateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

// ==== Dots rendering ====

HRESULT DotsRendering::CsData::updateAvs( const AvsState& ass )
{
	return updateValue( sampleV, ass.sampleV() );
}

HRESULT SimpleDots::render( RenderTargets& rt )
{
	constexpr UINT dotsCount = 768;
	if( !dotsBuffer )
		CHECK( dotsBuffer.create( sizeof( Vector2 ), dotsCount ) );

	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	// Calculate dots positions, with the CS
	const UINT uavSlot = renderer.compute().bindDotsPosition;
	bindUav( uavSlot, dotsBuffer.uav() );
	context->Dispatch( dotsCount / 256, 1, 1 );
	bindUav( uavSlot );

	// Render the sprites
	const UINT srvSlot = renderer.vertex().bindDots;
	bindResource<eStage::Vertex>( srvSlot, dotsBuffer.srv() );
	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->Draw( dotsCount, 0 );
	bindResource<eStage::Vertex>( srvSlot );

	return S_OK;
}

// ==== Solid rendering ====

HRESULT SolidRendering::VsData::updateAvs( const AvsState& avs )
{
	// TODO: positions
	return S_FALSE;
}

HRESULT SolidRendering::PsData::updateAvs( const AvsState& avs )
{
	return updateValue( sampleV, avs.sampleV() );
}

HRESULT SimpleSolid::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );
	return S_OK;
}

// ==== The effect itself ====

Simple::Simple( AvsState *pState ) :
	EffectBase1( pState ),
	m_impl( std::monostate{} )
{
	replaceStyleIfNeeded();
}

bool Simple::replaceStyleIfNeeded()
{
	const eSimpleRenderStyle rs = getRenderStyle( avs->effect );
	switch( rs )
	{
	case eSimpleRenderStyle::Dots:
		if( std::holds_alternative<SimpleDots>( m_impl ) )
			return false;
		m_pImpl = &m_impl.emplace<SimpleDots>( avs );
		return true;
	case eSimpleRenderStyle::Solid:
		if( std::holds_alternative<SimpleSolid>( m_impl ) )
			return false;
		m_pImpl = &m_impl.emplace<SimpleSolid>( avs );
		return true;
	}
	__debugbreak();
	return false;
}

HRESULT Simple::shouldRebuildState()
{
	// All rendering use the same state data (cycling colors), no need to rebuild it when user changes these style
	replaceStyleIfNeeded();
	return EffectBase1::shouldRebuildState();
}