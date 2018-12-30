#pragma once
#include "../Render/Binder.h"

namespace Hlsl
{
	// A base structure for effect's dynamic stage data.
	template<eStage stage>
	struct ShaderBaseStruct
	{
		static constexpr eStage shaderStage = stage;
		ShaderBaseStruct() = default;
		ShaderBaseStruct( const ShaderBaseStruct& ) = delete;

		static constexpr bool updateBindings( Binder& binder ) { return false; }

		template<class TAvsState>
		static constexpr HRESULT updateAvs( const TAvsState& as ) { return S_FALSE; }

		template<class TEffectState>
		static constexpr HRESULT updateDx( const TEffectState& dx ) { return S_FALSE; }
	};
}