#pragma once
#include "stdafx.h"
#include "Mosaic.h"
#include <Resources/dynamicCBuffer.h>

IMPLEMENT_EFFECT( Mosaic, C_MosaicClass )

MosaicStructs::MosaicCb MosaicStructs::AvsState::update( bool isBeat )
{
	if( nF )
	{
		nF--;
		if( nF )
		{
			int a = abs( quality - quality2 ) / durFrames;
			thisQuality += a * ( quality2 > quality ? -1 : 1 );
		}
	}

	if( onbeat && isBeat )
	{
		thisQuality = quality2;
		nF = durFrames;
	}
	else if( !nF )
		thisQuality = quality;

	const float2 screenSize = floatSize( getRenderSize() );
	if( thisQuality >= 100 )
		return MosaicCb{ screenSize, 0 };

	const float screenSizeScalar = std::max( screenSize.x, screenSize.y );

	const float cells = lerp( 1, 99, (float)thisQuality, 1, screenSizeScalar / 2 );
	const float cellsX = roundf( cells * screenSize.x / screenSizeScalar );
	const float cellsY = roundf( cells * screenSize.y / screenSizeScalar );

	// Direct3D counts levels from backwards.
	const float lod = log2f( screenSizeScalar ) - log2f( cells );
	return MosaicCb{ float2{ cellsX, cellsY }, lod };
}

HRESULT Mosaic::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled || !rt.lastWritten() )
		return S_FALSE;

	CHECK( m_texture.update( rt.lastWritten() ) );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	auto cb = avs->update( isBeat );
	if( cb.LOD <= 0 )
	{
		bindShader<eStage::Pixel>( StaticResources::copyTexture );
	}
	else
	{
		CHECK( updateCBuffer( m_cb, &cb, sizeof( cb ) ) );
		bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );
		bindSampler<eStage::Pixel>( renderer.pixel().bindSampler );	// <Default sampler state is what we need here
	}

	BoundSrv<eStage::Pixel> bound;
	if( avs->blend )
	{
		CHECK( rt.blendToNext( renderer.pixel().bindPrevFrame, bound ) );
		omBlend( eBlend::Add );
	}
	else if( avs->blendavg )
	{
		CHECK( rt.blendToNext( renderer.pixel().bindPrevFrame, bound ) );
		omCustomBlend();
	}
	else
	{
		CHECK( rt.writeToNext( renderer.pixel().bindPrevFrame, bound, false ) );
		omBlend( eBlend::None );
	}

	drawFullscreenTriangle( false );
	return S_OK;
}