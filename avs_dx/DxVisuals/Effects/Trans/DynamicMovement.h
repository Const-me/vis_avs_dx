#pragma once
#include "../EffectBase.h"

class DynamicMovement : public EffectBase
{
public:
	DynamicMovement();
	~DynamicMovement();

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;
};