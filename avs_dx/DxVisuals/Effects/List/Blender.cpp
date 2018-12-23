#include "stdafx.h"
#include "Blender.h"
#include "../../Resources/staticResources.h"

Blender::Blender()
{
	blendShader.data().source = 11;
	blendShader.data().dest = 12;
}

HRESULT Blender::blend( RenderTargets& source, RenderTargets& dest, eBlendMode mode, float blendVal )
{
	if( eBlendMode::Ignore == mode )
		return S_FALSE;

	RenderTarget& src = source.lastWritten();

	if( eBlendMode::Replace == mode )
	{
		RenderTarget& dst = dest.lastWritten();
		if( !dst )
			CHECK( dst.create() );

		if( src )
		{
			src.copyTo( dst );
			return S_FALSE;
		}
		dst.clear();
		return S_FALSE;
	}

	if( (uint8_t)mode > (uint8_t)eBlendMode::Minimum )
		return E_INVALIDARG;
	if( mode == eBlendMode::Buffer )
		return E_NOTIMPL;

	// Do the custom blending
	CHECK( ensureShader( mode, blendVal ) );

	setShaders( StaticResources::fullScreenTriangle, nullptr, blendShader );

	const UINT bindSource = blendShader.data().source;
	const UINT bindDest = blendShader.data().dest;

	CHECK( dest.writeToNext( bindDest, false ) );

	if( src )
		src.bindView( bindSource );
	else
		bindResource<eStage::Pixel>( bindSource, StaticResources::blackTexture );

	omDontBlend();
	blendShader.bind();
	drawFullscreenTriangle();

	bindResource<eStage::Pixel>( bindSource );
	bindResource<eStage::Pixel>( bindDest );
	return S_OK;
}

HRESULT Blender::ensureShader( eBlendMode mode, float blendVal )
{
	if( blendShader )
	{
		if( blendShader.data().blend == (uint8_t)mode && blendShader.data().blendVal == blendVal )
			return S_FALSE;

		blendShader.dropShader();
	}

	blendShader.data().blend = (uint8_t)mode;
	blendShader.data().blendVal = blendVal;

	CHECK( blendShader.compile( Hlsl::includes(), 0 ) );
	return S_OK;
}