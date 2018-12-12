#pragma once
#include "../Hlsl/StateShaderTemplate.h"
#include "../Hlsl/StateMacroValues.h"

struct EffectStateShader
{
	const StateShaderTemplate* shaderTemplate = nullptr;
	UINT stateSize = 0;
	Hlsl::StateMacroValues values;
};