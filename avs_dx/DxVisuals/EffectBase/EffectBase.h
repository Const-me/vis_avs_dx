#pragma once
#include "../iEffect.h"
class EffectStateBuilder;

class EffectBase: public iEffect
{
public:
	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;
};