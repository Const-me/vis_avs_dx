#pragma once
#include "EffectsResources.h"
#include "../Hlsl/Target.h"

class Binder
{
public:
	// Reserve shader slot for the specified resource, return string like "t4" that can be compiled into the shader code to access that resource
	CStringA bind( ResourceHandle resource, eTarget pipelineStage );
};