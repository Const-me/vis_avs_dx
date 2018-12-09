#pragma once
#include "../EffectBase.h"

class DynamicMovement : public EffectBase
{
public:
	DynamicMovement();
	~DynamicMovement();

	HRESULT build( EffectBuilder &builder ) override;
};