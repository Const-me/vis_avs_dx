#include "stdafx.h"
#include "Simple.h"

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

namespace
{
	eSource source( int effect )
	{
		return ( effect & 3 ) > 1 ? eSource::Spectrum : eSource::Wave;
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

HRESULT DotsRendering::CsData::updateValues( const AvsState& ass, int stateOffset )
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

	bindShaders();

	// Calculate dots positions, with the CS
	const UINT uavSlot = data<eStage::Compute>().bindDotsPosition;
	bindUav( uavSlot, dotsBuffer.uav() );
	context->Dispatch( 3, 1, 1 );

	// Render the sprites
	const UINT srvSlot = data<eStage::Vertex>().bindDots;
	bindResource<eStage::Vertex>( srvSlot, dotsBuffer.srv() );
	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->DrawIndexed( 768, 0, 0 );

	return S_OK;
}