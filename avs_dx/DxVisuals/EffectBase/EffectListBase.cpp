#include "stdafx.h"
#include "RootEffect.h"
#include "EffectList.h"

class C_RenderListClass;

// Class factory is slightly more complex here: creating different classes depending on whether this effect is the root or just a list inside the preset.
template<> HRESULT createDxEffect<C_RenderListClass>( void* pState, std::unique_ptr<iEffect>& dest )
{
	const EffectListBase::AvsState* pStateBase = ( EffectListBase::AvsState*)pState;
	if( pStateBase->isroot )
		return EffectImpl<RootEffect>::create( pState, dest );

	return EffectImpl<EffectList>::create( pState, dest );
};

EffectBase* EffectListBase::T_RenderListType::dxEffect() const
{
	if( !render->dxEffect )
		return nullptr;
	iEffect* p = render->dxEffect.get();
	return static_cast<EffectBase*>( p );
}