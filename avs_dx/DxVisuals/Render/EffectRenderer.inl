#pragma once

namespace
{
	// Horrible metaprogramming + macros below. I wish I had compile-time reflection instead. VC++ proprietary __if_exists helps but only slightly so.
	enum eShaderKind : uint8_t
	{
		None,
		Static,
		Dynamic
	};

#define QUERY_SHADER_KIND( funcName, DataClass, staticFuncName )    \
	template<class T>                                               \
	constexpr eShaderKind funcName()                                \
	{                                                               \
		__if_exists( T::DataClass )                                 \
		{                                                           \
			return eShaderKind::Dynamic;                            \
		}                                                           \
		__if_exists( T::staticFuncName )                            \
		{                                                           \
			return eShaderKind::Static;                             \
		}                                                           \
		return eShaderKind::None;                                   \
	}

	QUERY_SHADER_KIND( computeShaderKind, CsData, computeShader );
	QUERY_SHADER_KIND( vertexShaderKind, VsData, vertexShader );
	QUERY_SHADER_KIND( geometryShaderKind, GsData, geometryShader );
	QUERY_SHADER_KIND( pixelShaderKind, PsData, pixelShader );

#undef QUERY_SHADER_KIND

	template<class T, eStage stage, eShaderKind kind>
	struct ShaderTypeHelper;

	template<class T, eStage stage>
	struct ShaderTypeHelper<T, stage, eShaderKind::None>
	{
		using Type = NoShader<stage>;
		static constexpr bool ctorArg() { return false; }
	};

#define DEFINE_STATIC_SHADER( stage, staticFuncName )               \
	template<class T>                                               \
	struct ShaderTypeHelper<T, stage, eShaderKind::Static>          \
	{                                                               \
		using Type = StaticShader<stage>;                           \
		static IUnknown* ctorArg() { return T::staticFuncName(); }  \
	}

	DEFINE_STATIC_SHADER( eStage::Compute, computeShader );
	DEFINE_STATIC_SHADER( eStage::Vertex, vertexShader );
	DEFINE_STATIC_SHADER( eStage::Geometry, geometryShader );
	DEFINE_STATIC_SHADER( eStage::Pixel, pixelShader );

#undef DEFINE_STATIC_SHADER

#define DEFINE_DYNAMIC_SHADER( stage, DataClass )                   \
	template<class T>                                               \
	struct ShaderTypeHelper<T, stage, eShaderKind::Dynamic>         \
	{                                                               \
		using Type = Shader<typename T::DataClass>;                 \
		static constexpr bool ctorArg() { return false; }           \
	}

	DEFINE_DYNAMIC_SHADER( eStage::Compute, CsData );
	DEFINE_DYNAMIC_SHADER( eStage::Vertex, VsData );
	DEFINE_DYNAMIC_SHADER( eStage::Geometry, GsData );
	DEFINE_DYNAMIC_SHADER( eStage::Pixel, PsData );

#undef DEFINE_DYNAMIC_SHADER
}