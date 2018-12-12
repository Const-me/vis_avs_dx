#pragma once
#include "../Hlsl/StateShaderTemplate.h"
#include "../Hlsl/Defines.h"

struct EffectStateShader
{
	const StateShaderTemplate* shaderTemplate = nullptr;
	UINT stateSize = 0;
	Hlsl::Defines values;
};