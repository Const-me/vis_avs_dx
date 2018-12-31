#include "stdafx.h"
#include "../../EffectImpl.hpp"
#include "SeparableConvolution.h"

class BlurEffect : public EffectBase
{
public:
	struct AvsState
	{
		int enabled;
	};

	BlurEffect( AvsState* pState ) : avs( *pState ) { }

private:

	DECLARE_EFFECT()

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	AvsState& avs;
	SeparableConvolution m_convolution;
};

IMPLEMENT_EFFECT( BlurEffect, C_BlurClass )

HRESULT BlurEffect::updateParameters( Binder& binder )
{
	const ConvolutionKernel *pKernel = nullptr;
	switch( avs.enabled )
	{
	case 2:
		pKernel = &smallBlurKernel;
		break;
	case 3:
		pKernel = &largeBlurKernel;
		break;
	case 1:
		pKernel = &mediumBlurKernel;
		break;
	default:
		return S_FALSE;
	}

	return m_convolution.update( binder, pKernel );
}

HRESULT BlurEffect::render( bool isBeat, RenderTargets& rt )
{
	if( 0 == avs.enabled )
		return S_FALSE;
	return m_convolution.run( rt );
}