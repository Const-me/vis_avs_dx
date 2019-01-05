#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct FadeoutStructs
{
	struct AvsState
	{
		int fadelen, color;
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::Fadeout::FadeoutPS>
	{
		PsData() = default;
		PsData( const AvsState& avs )
		{
			color = float3FromColor( avs.color );
			fadelen = (float)avs.fadelen * ( 1.0f / 256.0f );
		}
	};

	using StateData = EmptyStateData;
};

class Fadeout : public EffectBase1<FadeoutStructs>
{
public:
	Fadeout( AvsState *pState ) : tBase( pState ) { }

private:
	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Fadeout, C_FadeOutClass )

HRESULT Fadeout::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );
	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
};