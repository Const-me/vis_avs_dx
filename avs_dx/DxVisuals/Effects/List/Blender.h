#pragma once
#include "../../Resources/RenderTargets.h"
#include "../../Render/Shader.hpp"
#include "../shadersCode.h"

class Blender
{
	Shader<Hlsl::List::BlenderPS> blendShader;

	HRESULT ensureShader( uint8_t mode, float blendVal );

public:

	Blender();

	HRESULT blend( RenderTargets& source, RenderTargets& dest, uint8_t mode, float blendVal );
};