#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>

struct InterleaveStructs
{
	struct AvsState
	{
		int x, y;
		int enabled;
		int color;
		int blend;
		int blendavg;
		int onbeat, x2, y2, beatdur;
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::Interleave::InterleavePS>
	{
		PsData() = default;
		PsData( const AvsState& avs )
		{
			color = float3FromColor( avs.color );
		}
	};

	struct StateData : public Hlsl::Trans::Interleave::InterleaveState
	{
		static constexpr UINT stateSize() { return 4; }

		StateData( const AvsState& avs )
		{
			beatdur = avs.beatdur;
			sizeNormal = CSize{ avs.x, avs.y };
			if( avs.onbeat )
				sizeOnBeat = CSize{ avs.x2, avs.y2 };
			else
				sizeOnBeat = sizeNormal;
		}
	};
};

class Interleave : public EffectBase1<InterleaveStructs>
{
public:
	Interleave( AvsState *pState ) : tBase( pState ) { }

private:
	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Interleave, C_InterleaveClass )

HRESULT Interleave::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	if( avs->blend )
		omBlend( eBlend::Add );
	else if( avs->blendavg )
		omCustomBlend( 0.5f );
	else
		omBlend( eBlend::None );

	drawFullscreenTriangle( false );
	return S_OK;
};