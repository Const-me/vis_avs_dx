#pragma once
#include "../EffectImpl.hpp"

class ColorModifier : public EffectBase
{
public:
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override
	{
		return E_NOTIMPL;
	}

	DECLARE_EFFECT( ColorModifier );
};