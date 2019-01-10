#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>

struct WaterStructs
{
	struct AvsState
	{
		int enabled;
	};
	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Trans::Water::WaterPS;

	using StateData = EmptyStateData;
};

class Water : public EffectBase1<WaterStructs>, public ResizeHandler
{
public:
	Water( AvsState *pState ) : tBase( pState ) { }

private:
	DECLARE_EFFECT()

	RenderTarget m_lastFrameOutput;

	void bindResources() override;

	void onRenderSizeChanged() override
	{
		m_lastFrameOutput.destroy();
	}

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Water, C_WaterClass )

void Water::bindResources()
{
	bindResource<eStage::Pixel>( renderer.pixel().bindLastOutput, m_lastFrameOutput.srv() );
}

HRESULT Water::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;
	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	if( !m_lastFrameOutput )
	{
		CHECK( m_lastFrameOutput.create( getRenderSize(), false, false ) );
		if( rt.lastWritten() )
			context->CopyResource( m_lastFrameOutput.texture(), rt.lastWritten().texture() );
		bindResources();
		return S_OK;
	}

	BoundPsResource bound;
	CHECK( rt.writeToNext( bound ) );
	omBlend( eBlend::None );
	drawFullscreenTriangle( false );

	context->CopyResource( m_lastFrameOutput.texture(), rt.lastWritten().texture() );

	return S_OK;
};