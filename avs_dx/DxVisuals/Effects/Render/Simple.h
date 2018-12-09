#pragma once
#include "../EffectBase.h"

struct Simple: public EffectBase
{
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;
};