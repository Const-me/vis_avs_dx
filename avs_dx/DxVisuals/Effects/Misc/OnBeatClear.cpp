#include "stdafx.h"
#include "../EffectImpl.hpp"

struct OnBeatClearStructs
{
	struct AvsState
	{
		int nf, cf, df;
		int color, blend;

		bool update( bool isBeat );
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	};

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Misc::ClearScreenPS>
	{
		PsData() = default;
		PsData( const AvsState& s )
		{
			color = float3FromColor( s.color );
		}
	};
};

class OnBeatClear : public EffectBase1<OnBeatClearStructs>
{
public:
	OnBeatClear( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

private:
	uint32_t m_frameCounter = 0;
	HRESULT buildState( EffectStateShader& ess ) override;
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( OnBeatClear, C_NFClearClass )

HRESULT OnBeatClear::buildState( EffectStateShader& ess )
{
	m_frameCounter = 0;
	return __super::buildState( ess );
}

bool OnBeatClearStructs::AvsState::update( bool isBeat )
{
	if( !isBeat )
	{
		if( ++df >= nf )
			df = 0;
		return false;
	}

	if( nf && ++cf >= nf )
	{
		cf = df = 0;
		return true;
	}

	return false;
}

HRESULT OnBeatClear::render( bool isBeat, RenderTargets& rt )
{
	if( !rt.lastWritten() )
		return S_FALSE;

	if( !avs->update( isBeat ) )
		return S_FALSE;

	if( 0 == avs->blend )
	{
		rt.lastWritten().clear( float3FromColor( avs->color ) );
		return S_OK;
	}

	omCustomBlend( 0.5f );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	CHECK( rt.writeToLast( false ) );
	drawFullscreenTriangle( false );
	return S_OK;
}