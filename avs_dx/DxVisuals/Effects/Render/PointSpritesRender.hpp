#pragma once
#include "../shadersCode.h"

// Geometry + pixel shaders to render a collection of point sprites.
struct PointSpritesRender
{
	using GsData = Hlsl::Render::PointSpriteGS;

	static ID3D11PixelShader* pixelShader()
	{
		return StaticResources::pointSprite;
	}
};