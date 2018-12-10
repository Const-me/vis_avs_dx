#include "stdafx.h"
#include "RootEffect.h"
#include "EffectList.h"

class C_RenderListClass;

template<> HRESULT createDxEffect<C_RenderListClass>( void* pState, std::unique_ptr<EffectBase>& dest ) 
{
	const EffectListBase::AvsState* pStateBase = ( EffectListBase::AvsState*)pState;
	if( pStateBase->isroot )
		return EffectImpl<RootEffect>::create( pState, dest );

	return EffectImpl<EffectList>::create( pState, dest );
};