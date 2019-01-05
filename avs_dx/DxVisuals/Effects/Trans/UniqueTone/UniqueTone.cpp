#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct UniqueToneStructs
{
	struct AvsState
	{
		int enabled;
		int invert;
		int color;
		int blend, blendavg;
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::UniqueTone::UniqueTonePS>
	{
		PsData() = default;
		PsData( const AvsState& avs )
		{
			invert = avs.invert ? 1 : 0;
			color = float3FromColor( avs.color );
		}
	};

	using StateData = EmptyStateData;
};

class UniqueTone : public EffectBase1<UniqueToneStructs>
{
public:
	UniqueTone( AvsState *pState ) : tBase( pState ) { }

private:
	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( UniqueTone, C_OnetoneClass )

HRESULT UniqueTone::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;
	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	BoundPsResource bound;
	if( avs->blend )
	{
		CHECK( rt.blendToNext( bound ) );
		omBlend( eBlend::Add );
	}
	else if( avs->blendavg )
	{
		CHECK( rt.blendToNext( bound ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( bound ) );
		omBlend( eBlend::None );
	}
	drawFullscreenTriangle( false );
	return S_OK;
};