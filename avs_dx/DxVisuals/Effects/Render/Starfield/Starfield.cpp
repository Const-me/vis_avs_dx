#include "stdafx.h"
#include "Starfield.h"
#include <../InteropLib/interop.h>

IMPLEMENT_EFFECT( Starfield, C_StarField );

// Max.limit of the count, the size of the buffers
constexpr UINT nStarsCapacity = 4096;

Starfield::Starfield( AvsState *pState ):
	FacticleFx(pState, 16, nStarsCapacity, "StarfieldUpdate" )
{ }

HRESULT Starfield::initializedState()
{
	CHECK( __super::initializedState() )
	// Refresh stars count on the next render()
	prevStars = 0;
	nStars = 0;
	return S_OK;
}

StarfieldStructs::StateData::StateData( const AvsState& s )
{
	onbeat = s.onbeat ? true : false;
	spdBeat = s.spdBeat;
	WarpSpeed = s.WarpSpeed;
	durFrames = s.durFrames;
}

HRESULT StarfieldStructs::VsData::updateAvs( const AvsState& ass )
{
	float4 rgba = float4FromColor( ass.color, 1 );
	if( ass.blendavg )
		rgba *= 0.5f;
	return updateValue( starsColor, rgba );
}

HRESULT Starfield::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( prevStars != avs->MaxStars_set )
	{
		prevStars = avs->MaxStars_set;
		const CSize sz = getRenderSize();
		nStars = MulDiv( avs->MaxStars_set, sz.cx * sz.cy, 512 * 384 );
	}

	omBlend( avs->blend ? eBlend::Add : eBlend::Premultiplied );
	CHECK( rt.writeToLast( false ) );
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	return renderParticles( nStars );
}