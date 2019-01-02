#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct ColorClipStructs
{
	struct AvsState
	{
		int enabled;
		int color_clip, color_clip_out, color_dist;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::ColorClip::ColorClipPS>
	{
		PsData() = default;
		PsData( const AvsState &s )
		{
			enabled = s.enabled;
			color_clip = float3FromColor( s.color_clip );
			color_clip_out = float3FromColor( s.color_clip_out );
			color_dist = (float)s.color_dist / 127.0f;
		}
	};
};

class ColorClip : public EffectBase1<ColorClipStructs>
{
public:
	ColorClip( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( ColorClip, C_ContrastEnhanceClass )

HRESULT ColorClip::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	BoundSrv<eStage::Pixel> bound;
	CHECK( rt.writeToNext( renderer.pixel().bindPrevFrame, bound, false ) );

	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}