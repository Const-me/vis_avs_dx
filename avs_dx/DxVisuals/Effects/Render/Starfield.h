#pragma once
#include "../EffectBase.h"

class Starfield : public EffectBase
{
public:
	Starfield();
	~Starfield();

	HRESULT build( EffectBuilder &builder ) override;
};