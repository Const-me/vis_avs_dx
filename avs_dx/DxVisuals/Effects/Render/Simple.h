#pragma once
#include "../EffectBase.h"

class Simple: public EffectBase
{
public:
	HRESULT build( EffectBuilder &builder ) override;
};