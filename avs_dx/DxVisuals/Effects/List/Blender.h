#pragma once
#include "../../Resources/RenderTargets.h"
#include "../../Render/Shader.hpp"
#include "../shadersCode.h"

enum struct eBlendMode : uint8_t
{
	Ignore = 0,
	Replace = 1,
	Fifty = 2,
	Maximum = 3,
	Additive = 4,
	Subtractive1 = 5,
	Subtractive = 6,
	LineInterleave = 7,
	PixelInterleave = 8,
	Xor = 9,
	Adjustable = 10,
	Multiply = 11,
	Buffer = 12,
	Minimum = 13
};

class Blender
{
	Shader<Hlsl::List::BlenderPS> blendShader;

	HRESULT ensureShader( eBlendMode mode, float blendVal );

public:

	static bool modeUsesShader( eBlendMode mode );

	bool updateBindings( Binder& binder );

	HRESULT blend( RenderTargets& source, RenderTargets& dest, eBlendMode mode, float blendVal );
};