#pragma once
#include <type_traits>
#include "../DxVisuals/iRootEffect.h"

// Class factory function for DirectX effects. The template parameter is the class implementing the effect, from deep inside of the AVS implementation.
template<class AvsEffect>
HRESULT createDxEffect( void* pState, DxEffectPtr& dest );

#define CREATE_DX_EFFECT( firstStateVar ) createDxEffect<std::remove_reference<decltype(*this)>::type>( &firstStateVar, this->dxEffect )