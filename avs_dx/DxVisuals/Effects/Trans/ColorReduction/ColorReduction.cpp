#pragma once
#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct ColorReductionStructs
{
	struct AvsState
	{
		uint32_t levels;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::ColorReduction::ColorReductionPS>
	{
		PsData() = default;
		PsData( const AvsState &s )
		{
			levels = s.levels;
		}
	};
};

class ColorReduction : public EffectBase1<ColorReductionStructs>
{
public:
	ColorReduction( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( ColorReduction, C_ColorReduction )

HRESULT ColorReduction::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	BoundSrv<eStage::Pixel> bound;
	CHECK( rt.writeToNext( renderer.pixel().bindPrevFrame, bound, false ) );

	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}