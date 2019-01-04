#pragma once
#include "../shadersCode.h"

// Geometry + pixel shaders to render a collection of point sprites.
struct PointSpritesRender
{
	using GsData = Hlsl::Render::PointSpriteGS;

	using PsData = Hlsl::Render::PointSpritePS;
};