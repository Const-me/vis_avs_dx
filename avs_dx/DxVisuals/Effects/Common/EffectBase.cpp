#include "stdafx.h"
#include "EffectBase.h"
#include <RootEffect.h>

const EffectBase::Metadata& EffectBase::metadata()
{
	__debugbreak(); // Must override
	return *(Metadata*)( nullptr );
}

ID3D11Buffer* EffectBase::stateBuffer() const
{
	return m_pRoot->stateBuffer();
}