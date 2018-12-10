#pragma once
#include <memory>
#include <type_traits>
#include "../DxVisuals/EffectBase/EffectBase.h"

template<class AvxEffect>
HRESULT createDxEffect( void* pState, std::unique_ptr<EffectBase>& dest );

#define CREATE_DX_EFFECT( firstStateVar ) createDxEffect<std::remove_reference<decltype(*this)>::type>( &firstStateVar, this->dxEffect )