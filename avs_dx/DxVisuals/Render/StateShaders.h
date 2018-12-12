#pragma once
#include "EffectStateShader.hpp"

struct StateShaders
{
	CComPtr<ID3D11ComputeShader> init, update, updateOnBeat;

	HRESULT compile( const std::vector<EffectStateShader> &effects, UINT& totalStateSize );
};