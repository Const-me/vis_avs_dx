#pragma once
#include "EffectImpl.hpp"

class EffectList: public EffectBase
{
public:
	struct AvsState
	{

	};
	AvsState* const avs;

	EffectList( AvsState* pState ) : avs( pState ) { }

	HRESULT stateDeclarations( EffectStateBuilder &builder ) { return E_NOTIMPL; }
};