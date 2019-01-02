#include "stdafx.h"
#include "ColorFade.h"

IMPLEMENT_EFFECT( ColorFade, C_ColorFadeClass )

void ColorFadeStructs::AvsState::update( bool isBeat )
{
	if( faderpos[ 0 ] < faders[ 0 ] ) faderpos[ 0 ]++;
	if( faderpos[ 1 ] < faders[ 2 ] ) faderpos[ 1 ]++;
	if( faderpos[ 2 ] < faders[ 1 ] ) faderpos[ 2 ]++;
	if( faderpos[ 0 ] > faders[ 0 ] ) faderpos[ 0 ]--;
	if( faderpos[ 1 ] > faders[ 2 ] ) faderpos[ 1 ]--;
	if( faderpos[ 2 ] > faders[ 1 ] ) faderpos[ 2 ]--;

	if( !( enabled & 4 ) )
	{
		faderpos[ 0 ] = faders[ 0 ];
		faderpos[ 1 ] = faders[ 1 ];
		faderpos[ 2 ] = faders[ 2 ];
	}
	else if( isBeat && ( enabled & 2 ) )
	{
		faderpos[ 0 ] = ( rand() % 32 ) - 6;
		faderpos[ 1 ] = ( rand() % 64 ) - 32;
		if( faderpos[ 1 ] < 0 && faderpos[ 1 ] > -16 ) faderpos[ 1 ] = -32;
		if( faderpos[ 1 ] >= 0 && faderpos[ 1 ] < 16 ) faderpos[ 1 ] = 32;
		faderpos[ 2 ] = ( rand() % 32 ) - 6;
	}
	else if( isBeat )
	{
		faderpos[ 0 ] = beatfaders[ 0 ];
		faderpos[ 1 ] = beatfaders[ 1 ];
		faderpos[ 2 ] = beatfaders[ 2 ];
	}
}

std::array<Vector4, 4> ColorFadeStructs::AvsState::shaderConstants() const
{
	constexpr float mul = (float)( 1.0 / 255.0 );
	const float fs1 = mul * (float)faderpos[ 0 ];
	const float fs2 = mul * (float)faderpos[ 1 ];
	const float fs3 = mul * (float)faderpos[ 2 ];

	std::array<Vector4, 4> res;
	res[ 0 ].x = fs3;
	res[ 0 ].y = fs2;
	res[ 0 ].z = fs1;
	res[ 0 ].w = 0;

	res[ 1 ].x = fs2;
	res[ 1 ].y = fs1;
	res[ 1 ].z = fs3;
	res[ 1 ].w = 0;

	res[ 2 ].x = fs1;
	res[ 2 ].y = fs3;
	res[ 2 ].z = fs2;
	res[ 2 ].w = 0;

	res[ 3 ].x = fs3;
	res[ 3 ].y = fs3;
	res[ 3 ].z = fs3;
	res[ 3 ].w = 0;

	return res;
}

HRESULT ColorFade::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	avs->update( isBeat );
	auto data = avs->shaderConstants();
	CHECK( updateCBuffer( m_cb, data.data(), sizeof( data ) ) );

	bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );
	BoundSrv<eStage::Pixel> bound;
	CHECK( rt.writeToNext( renderer.pixel().bindPrevFrame, bound, false ) );

	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}