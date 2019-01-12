#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>
#include <../../avs/vis_avs/resource.h>

struct ChannelShiftStructs
{
	struct AvsState
	{
		volatile int mode;
		volatile int onbeat;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::ChannelShift::ChannelShiftPS>
	{
		PsData() = default;
		PsData( const AvsState& s )
		{
			mode = s.mode;
		}
	};
};

class ChannelShift : public EffectBase1<ChannelShiftStructs>
{
public:
	ChannelShift( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( ChannelShift, C_ChannelShiftClass )

HRESULT ChannelShift::render( bool isBeat, RenderTargets& rt )
{
	if( isBeat && avs->onbeat )
	{
		constexpr int modes[] = { IDC_RGB, IDC_RBG, IDC_GBR, IDC_GRB, IDC_BRG, IDC_BGR };
		const int newMode = modes[ rand() % 6 ];
		if( newMode != avs->mode )
		{
			avs->mode = newMode;
			CHECK( renderer.pixel().updateAvs( *avs ) );
			CHECK( renderer.compileShader<eStage::Pixel>( stateOffset() ) );
		}
	}

	omBlend( eBlend::None );
	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	drawFullscreenTriangle( false );
	return S_OK;
}