#pragma once
#include "../Render/Binder.h"

namespace Hlsl
{
	// A base structure for effect's stage data.
	template<eStage stage>
	struct ShaderBaseStruct
	{
		static constexpr eStage shaderStage = stage;
		ShaderBaseStruct() = default;
		ShaderBaseStruct( const ShaderBaseStruct& ) = delete;
	};
}