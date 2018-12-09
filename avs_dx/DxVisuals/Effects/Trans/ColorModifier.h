#pragma once
#include "../EffectBase.h"

class ColorModifier : public EffectBase
{
public:
	ColorModifier();
	~ColorModifier();

	HRESULT build( EffectBuilder &builder ) override;
};