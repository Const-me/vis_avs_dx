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

	const float cellsLog = lerp( 1, 99, (float)thisQuality, -log2f( screenSizeScalar ), -0.5 );
	const float cells = screenSizeScalar * exp2f( cellsLog );
	const float cellsX = roundf( cells * screenSize.x / screenSizeScalar );
	const float cellsY = roundf( cells * screenSize.y / screenSizeScalar );

	// Direct3D counts levels from backwards.
	const float lod = log2f( screenSizeScalar ) - log2f( cells );
	return MosaicCb{ float2{ cellsX, cellsY }, lod };
}

Mosaic::Mosaic( AvsState* avs ) : EffectBase1( avs ),
	m_profileMipmaps( "MosaicMipmaps" )
{ }

HRESULT Mosaic::renderDisabled( RenderTargets& rt )
{
	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );

	BoundSrv<eStage::Pixel> bound;
	if( avs->blend )
		omBlend( eBlend::Add );
	else if( avs->blendavg )
		omCustomBlend();
	else
		return S_FALSE;

	CHECK( rt.blendToNext( 127, bound ) );

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

	CHECK( m_texture.update( rt.lastWritten() ) );
	m_profileMipmaps.mark();

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	bindResource<eStage::Pixel>( renderer.pixel().bindPrevFrame, m_texture.srv() );
	CHECK( updateCBuffer( m_cb, &cb, sizeof( cb ) ) );
	bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );

#ifdef DEBUG
	if( !m_sampler )
	{
		CD3D11_SAMPLER_DESC sd{ D3D11_DEFAULT };
		CHECK( device->CreateSamplerState( &sd, &m_sampler ) );
		bindSampler<eStage::Pixel>( renderer.pixel().bindSampler, m_sampler );
	}
#else
	// Default sampler state is what we need here
	bindSampler<eStage::Pixel>( renderer.pixel().bindSampler );
#endif

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