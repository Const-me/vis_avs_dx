#pragma once
#include "EffectImpl.hpp"

class EffectList: public EffectBase
{
public:
	DECLARE_EFFECT( EffectList );

	struct T_RenderListType;

	HRESULT stateDeclarations( EffectStateBuilder &builder ) { return E_NOTIMPL; }
};