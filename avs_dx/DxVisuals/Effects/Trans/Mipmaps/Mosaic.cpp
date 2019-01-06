#pragma once
#include "stdafx.h"
#include "Mosaic.h"
#include <Resources/dynamicBuffers.h>

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

	const float screenSizeScalar = eastl::max( screenSize.x, screenSize.y );

	const float cellsLog = lerp( 1, 99, (float)thisQuality, -log2f( screenSizeScalar ), -0.5 );
	const float cells = screenSizeScalar * exp2f( cellsLog );
	const float cellsX = roundf( cells * screenSize.x / screenSizeScalar );
	const float cellsY = roundf( cells * screenSize.y / screenSizeScalar );

	// Direct3D counts levels from backwards.
	const float lod = log2f( screenSizeScalar ) - log2f( cells );
	return MosaicCb{ float2{ cellsX, cellsY }, lod };
}

Mosaic::Mosaic( AvsState* avs ) : EffectBase1( avs ), MipMapsRenderer( "MosaicMipmaps" )
{ }

HRESULT Mosaic::renderDisabled( RenderTargets& rt )
{
	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );

	if( avs->blend )
		omBlend( eBlend::Add );
	else if( avs->blendavg )
		omCustomBlend();
	else
		return S_FALSE;

	BoundPsResource bound;
	CHECK( rt.blendToNext( bound ) );

	drawFullscreenTriangle( false );
	return S_OK;
}

HRESULT Mosaic::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled || !rt.lastWritten() )
		return S_FALSE;

	auto cb = avs->update( isBeat );
	if( cb.LOD <= 0 )
		return renderDisabled( rt );

	CHECK( updateMipmaps( rt.lastWritten() ) );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	CHECK( updateCBuffer( m_cb, &cb, sizeof( cb ) ) );
	bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );

	CHECK( bindMipmaps( renderer.pixel().bindSampler ) );

	BoundSrv<eStage::Pixel> bound;
	if( avs->blend )
		omBlend( eBlend::Add );
	else if( avs->blendavg )
		omCustomBlend();
	else
		omBlend( eBlend::None );

	CHECK( rt.writeToLast() );

	drawFullscreenTriangle( false );
	return S_OK;
}