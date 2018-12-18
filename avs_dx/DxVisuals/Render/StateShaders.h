#pragma once
#include "EffectStateShader.hpp"
#include "../Utils/events.h"

struct StateShaders: public iResizeHandler
{
	StateShaders() = default;
	~StateShaders();

	CComPtr<ID3D11ComputeShader> init, update, updateOnBeat;

	operator bool() const
	{
		return nullptr != init && nullptr != update && nullptr != updateOnBeat;
	}

	HRESULT compile( const std::vector<EffectStateShader> &effects, UINT& totalStateSize );

private:
	void onRenderSizeChanged() override;
};