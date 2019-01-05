#include "stdafx.h"
#include "Blender.h"
#include "../../Resources/staticResources.h"

bool Blender::modeUsesShader( eBlendMode mode )
{
	switch( mode )
	{
	case eBlendMode::Ignore:
	case eBlendMode::Replace:
	case eBlendMode::Buffer:
		return false;
	}
	return true;
}

bool Blender::updateBindings( Binder& binder )
{
	BoolHr hr = blendShader.update( binder, nullptr, nullptr );
	return hr.value();
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
			return S_OK;
		}
		dst.clear();
		return S_OK;
	}

	if( (uint8_t)mode > (uint8_t)eBlendMode::Minimum )
		return E_INVALIDARG;
	if( mode == eBlendMode::Buffer )
		return E_NOTIMPL;

	// Do the custom blending
	CHECK( ensureShader( mode, blendVal ) );

	setShaders( StaticResources::fullScreenTriangle, nullptr, blendShader.ptr( false ) );

	const UINT bindSource = blendShader.data().source;
	BoundPsResource boundDest;
	CHECK( dest.writeToNext( boundDest ) );

	bindResource<eStage::Pixel>( bindSource, src ? src.srv() : StaticResources::blackTexture.operator ->() );

	omBlend( eBlend::None );
	blendShader.bind( false );
	drawFullscreenTriangle();
	return S_OK;
}

HRESULT Blender::ensureShader( eBlendMode mode, float blendVal )
{
	if( blendShader.hasShader() )
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