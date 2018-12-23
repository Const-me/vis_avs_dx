#include "stdafx.h"
#include "Blender.h"
#include "../Resources/staticResources.h"

Blender::Blender()
{
	blendShader.data().source = 125;
	blendShader.data().dest = 126;
}

HRESULT Blender::blend( RenderTargets& source, RenderTargets& dest, uint8_t mode, float blendVal )
{
	// Ignore
	if( 0 == mode )
	{
		return S_FALSE;
	}

	RenderTarget& src = source.lastWritten();
	RenderTarget& dst = dest.lastWritten();

	if( !dst )
		CHECK( dst.create( getRenderSize() ) );

	// Replace
	if( 1 == mode )
	{
		if( src )
		{
			src.copyTo( dst );
			return S_FALSE;
		}
		dst.clear();
		return S_FALSE;
	}

	// Do the blending
	CHECK( ensureShader( mode, blendVal ) );

	setShaders( StaticResources::fullScreenTriangle, nullptr, blendShader );

	const UINT bindSource = blendShader.data().source;
	const UINT bindDest = blendShader.data().dest;

	if( src )
		src.bindView( bindSource );
	else
		bindResource<eStage::Pixel>( bindSource, StaticResources::blackTexture );

	CHECK( dest.writeToNext( bindDest, false ) );

	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->Draw( 3, 0 );

	bindResource<eStage::Pixel>( bindSource );
	bindResource<eStage::Pixel>( bindDest );
	return S_OK;
}

HRESULT Blender::ensureShader( uint8_t mode, float blendVal )
{
	if( blendShader )
	{
		if( blendShader.data().blend == mode && blendShader.data().blendVal == blendVal )
			return S_FALSE;

		blendShader.dropShader();
	}

	blendShader.data().blend = mode;
	blendShader.data().blendVal = blendVal;

	CHECK( blendShader.compile( Hlsl::includes(), 0 ) );
	return S_OK;
}