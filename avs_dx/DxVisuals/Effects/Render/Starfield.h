#pragma once
#include "../EffectImpl.hpp"

class Starfield : public EffectBase
{
public:
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override
	{
		return E_NOTIMPL;
	}

	DECLARE_EFFECT( Starfield );

	struct StarFormat;
};