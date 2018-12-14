#pragma once
#include "../Render/Binder.h"

namespace Hlsl
{
	template<eStage stage>
	struct ShaderBaseStruct
	{
		ShaderBaseStruct() = default;
		ShaderBaseStruct( const ShaderBaseStruct& ) = delete;
		bool updateBindings( Binder& binder ) { return false; }
		template<class TAvx>
		HRESULT update( const TAvx& a ) { return S_FALSE; }
		template<class TAvx>
		HRESULT defines( const TAvx& a ) { return S_FALSE; }
	};
}