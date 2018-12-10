#pragma once
#include <memory>
#include <type_traits>
#include "../DxVisuals/iEffect.h"

template<class AvxEffect>
HRESULT createDxEffect( void* pState, std::unique_ptr<iEffect>& dest );

#define CREATE_DX_EFFECT( firstStateVar ) createDxEffect<std::remove_reference<decltype(*this)>::type>( &firstStateVar, this->dxEffect )