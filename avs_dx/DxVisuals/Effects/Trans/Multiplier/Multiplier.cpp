#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct MultiplierStructs
{
	struct AvsState
	{
		int ml;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::Multiplier::MultiplierPS>
	{
		PsData() = default;
		PsData( const AvsState &s )
		{
			mode = (uint32_t)s.ml;
		}
	};
};

class Multiplier : public EffectBase1<MultiplierStructs>
{
public:
	Multiplier( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Multiplier, C_MultiplierClass )

HRESULT Multiplier::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );
	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}