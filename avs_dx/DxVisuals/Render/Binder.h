#pragma once
#include "EffectsResources.h"
#include "Stage.h"

class Binder
{
	// std::array<>
public:
	// Reserve shader slot for the specified resource, return string like "t4" that can be compiled into the shader code to access that resource
	const CStringA& bind( eStage pipelineStage, ID3D11ShaderResourceView* srv );
};