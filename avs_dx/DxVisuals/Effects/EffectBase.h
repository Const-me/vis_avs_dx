#pragma once
#include "../Render/EffectStateBuilder.h"

class EffectBase
{
public:
	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;
};