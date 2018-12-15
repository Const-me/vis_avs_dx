#pragma once
#include "../Render/Binder.h"

namespace Hlsl
{
	// A base structure for effect's stage data.
	template<eStage stage>
	struct ShaderBaseStruct
	{
		ShaderBaseStruct() = default;
		ShaderBaseStruct( const ShaderBaseStruct& ) = delete;
	};
}