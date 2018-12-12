#pragma once
#include "../Hlsl/StateShaderTemplate.h"
#include "../Hlsl/StateMacroValues.h"

struct StateBuilder
{
	CComPtr<ID3D11ComputeShader> init, update, updateOnBeat;

	struct Effect
	{
		const StateShaderTemplate* shaderTemplate;
		UINT stateSize;
		Hlsl::StateMacroValues values;
	};

	HRESULT compile( const std::vector<Effect> &effects );
};