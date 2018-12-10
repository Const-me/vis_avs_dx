#pragma once
class EffectStateBuilder;

class EffectBase
{
public:
	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;
};