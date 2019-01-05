#pragma once
#include "EffectStateShader.hpp"
#include "ShaderBase.h"

class StateShaders:
	public ShaderBase<eStage::Compute>
{
	__m128i m_hash;

public:
	StateShaders() = default;

	CComPtr<ID3D11ComputeShader> init;

	operator bool() const
	{
		return nullptr != init && hasShader();
	}

	HRESULT compile( const std::vector<EffectStateShader> &effects, UINT& totalStateSize );

protected:

	void onRenderSizeChanged() override;
};