#include "stdafx.h"
#include "../../EffectImpl.hpp"

struct FastBrightnessStruct
{
	struct AvsState
	{
		uint32_t dir;
	};
	static ID3D11VertexShader *vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}
	struct PsData : public Hlsl::Trans::Brightness::FastBrightnessPS
	{
		HRESULT updateAvs( const AvsState& s )
		{
			return updateValue( dir, s.dir );
		}
	};
	using StateData = EmptyStateData;
};

class FastBrightness : public EffectBase1<FastBrightnessStruct>
{
public:
	FastBrightness( AvsState* p ) : tBase( p ) { }

private:
	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( FastBrightness, C_FastBright )

HRESULT FastBrightness::render( bool isBeat, RenderTargets& rt )
{
	if( avs->dir > 1 )
		return S_FALSE;

	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );
	if( !renderer.bindShaders( false ) )
		return S_FALSE;
	omBlend( eBlend::None );
	drawFullscreenTriangle( false );
	return S_OK;
}