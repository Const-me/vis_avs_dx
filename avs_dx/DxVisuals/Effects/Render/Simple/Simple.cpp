#include "stdafx.h"
#include "Simple.h"

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

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
}

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

HRESULT DotsRendering::CsData::updateAvs( const AvsState& ass )
{
	if( ass.effect == m_effect )
		return S_FALSE;
	sampleV = ass.sampleV();
	// TODO: update the position

	m_effect = ass.effect;
	return S_OK;
}

HRESULT Simple::initializedState()
{
	// TODO: change based on the size
	constexpr UINT dotsCount = 768;
	if( !dotsBuffer )
		CHECK( dotsBuffer.create( sizeof( Vector2 ), dotsCount ) );

	return S_OK;
}

HRESULT Simple::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	// Calculate dots positions, with the CS
	const UINT uavSlot = renderer.compute().bindDotsPosition;
	bindUav( uavSlot, dotsBuffer.uav() );
	context->Dispatch( 3, 1, 1 );
	bindUav( uavSlot );

	// Render the sprites
	const UINT srvSlot = renderer.vertex().bindDots;
	bindResource<eStage::Vertex>( srvSlot, dotsBuffer.srv() );
	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->Draw( 768, 0 );
	bindResource<eStage::Vertex>( srvSlot );

	return S_OK;
}