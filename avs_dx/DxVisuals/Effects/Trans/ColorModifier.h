#pragma once
#include "../EffectBase.h"

class ColorModifier : public EffectBase
{
public:
	ColorModifier();
	~ColorModifier();

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;
};