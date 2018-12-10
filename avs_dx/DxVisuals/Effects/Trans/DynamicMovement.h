#pragma once
#include "../EffectImpl.hpp"

class DynamicMovement : public EffectBase
{
public:
	DECLARE_EFFECT( DynamicMovement )

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override
	{
		return E_NOTIMPL;
	}

	HRESULT render( RenderTargets& rt ) override { return E_NOTIMPL; }
};