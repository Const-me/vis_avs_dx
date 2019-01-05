#include "stdafx.h"
#include "../RootEffect.h"
#include "../Effects/List/EffectList.h"

class C_RenderListClass;

// Class factory is slightly more complex here: creating different classes depending on whether this effect is the root or just a list inside the preset.
template<> HRESULT createDxEffect<C_RenderListClass>( void* pState, const C_RBASE* pRBase )
{
	const EffectListBase::AvsState* pStateBase = ( EffectListBase::AvsState* )pState;
	if( pStateBase->isroot )
		return EffectImpl<RootEffect>::create( pState, pRBase );

	return EffectImpl<EffectList>::create( pState, pRBase );
};

// Similarly, need to implement two copies of metadata() method.

const char* const EffectImpl<RootEffect>::s_effectName = "RootEffect";
static const EffectBase::Metadata s_root{ "RootEffect", true };
const EffectBase::Metadata& RootEffect::metadata()
{
	return s_root;
}

const char* const EffectImpl<EffectList>::s_effectName = "EffectList";
static const EffectBase::Metadata s_list{ "EffectList", true };
const EffectBase::Metadata& EffectList::metadata()
{
	return s_list;
}