#include "stdafx.h"
#include "../RootEffect.h"
#include "../Effects/List/EffectList.h"

class C_RenderListClass;

// Class factory is slightly more complex here: creating different classes depending on whether this effect is the root or just a list inside the preset.
template<> HRESULT createDxEffect<C_RenderListClass>( void* pState, std::unique_ptr<iRootEffect>& dest )
{
	const EffectListBase::AvsState* pStateBase = ( EffectListBase::AvsState* )pState;
	if( pStateBase->isroot )
		return EffectImpl<RootEffect>::create( pState, dest );

	return EffectImpl<EffectList>::create( pState, dest );
};

// Similarly, need to implement two copies of metadata() method.
static const EffectBase::Metadata s_root{ "RootEffect", true };
const EffectBase::Metadata& RootEffect::metadata()
{
	return s_root;
}

static const EffectBase::Metadata s_list{ "EffectList", true };
const EffectBase::Metadata& EffectList::metadata()
{
	return s_list;
}