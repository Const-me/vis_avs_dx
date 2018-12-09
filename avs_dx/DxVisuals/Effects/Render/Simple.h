#pragma once
#include "../EffectBase.h"

class Simple: public EffectBase
{
public:
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;
};