#include "stdafx.h"
#include "../EffectImpl.hpp"

struct ClearScreenStructs
{
	struct AvsState
	{
		int enabled;
		int onlyfirst;
		int color;
		int fcounter;
		int blend, blendavg;
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

class ClearScreen : public EffectBase1<ClearScreenStructs>
{
public:
	ClearScreen( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

private:
	uint32_t m_frameCounter = 0;
	HRESULT buildState( EffectStateShader& ess ) override;
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( ClearScreen, C_ClearClass )

HRESULT ClearScreen::buildState( EffectStateShader& ess )
{
	m_frameCounter = 0;
	return __super::buildState( ess );
}

HRESULT ClearScreen::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	if( !rt.lastWritten() )
		return S_FALSE;

	if( avs->onlyfirst && m_frameCounter > 0 )
		return S_FALSE;

	m_frameCounter++;

	if( 0 == avs->blend && 0 == avs->blendavg )
	{
		rt.lastWritten().clear( float3FromColor( avs->color ) );
		return S_OK;
	}

	if( avs->blend )
		omBlend( eBlend::Add );
	else
		omCustomBlend( 0.5f );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	CHECK( rt.writeToLast( false ) );
	drawFullscreenTriangle( false );
	return S_OK;
}