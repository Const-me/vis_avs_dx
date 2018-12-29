#pragma once

namespace
{
	// Horrible metaprogramming + macros below. I wish I had compile-time reflection instead. VC++ proprietary __if_exists helps but only slightly so.

	enum eShaderKind : uint8_t
	{
		// The effect said nothing about this stage's shader
		None,
		// The effect defined a method returning a live shader, e.g. `static ID3D11VertexShader* vertexShader();`
		Static,
		// The effect defined a method returning DXBC bytecode of the shader, e.g. `static ByteRange pixelShaderBinary();`
		Binary,
		// The effect defined a structure describing a dynamically compiling shader, it has HLSL code that depends on user or system-provided values.
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
		__if_exists( T::staticFuncName##Binary )                    \
		{                                                           \
			return eShaderKind::Binary;                             \
		}                                                           \
		return eShaderKind::None;                                   \
	}

	QUERY_SHADER_KIND( computeShaderKind, CsData, computeShader );
	QUERY_SHADER_KIND( vertexShaderKind, VsData, vertexShader );
	QUERY_SHADER_KIND( geometryShaderKind, GsData, geometryShader );
	QUERY_SHADER_KIND( pixelShaderKind, PsData, pixelShader );

#undef QUERY_SHADER_KIND

	// T is effect structure passed into FxDef template argument of EffectRenderer
	template<class T, eStage stage, eShaderKind kind>
	struct ShaderTypeHelper;

	// Specialize that structure for various shader types

	// Empty shaders are trivial to specialize, we don't need to do anything for them.
	template<class T, eStage stage>
	struct ShaderTypeHelper<T, stage, eShaderKind::None>
	{
		using Type = NoShader<stage>;
		static constexpr bool ctorArg() { return false; }
	};

	// Static shaders and static binary shaders
#define DEFINE_STATIC_SHADER( stage, staticFuncName )                       \
	template<class T>                                                       \
	struct ShaderTypeHelper<T, stage, eShaderKind::Static>                  \
	{                                                                       \
		using Type = StaticShader<stage>;                                   \
		static IShader<stage>* ctorArg() { return T::staticFuncName(); }    \
	};                                                                      \
	template<class T>                                                       \
	struct ShaderTypeHelper<T, stage, eShaderKind::Binary>                  \
	{                                                                       \
		using Type = BinaryShader<stage>;                                   \
		static ByteRange ctorArg() { return T::staticFuncName##Binary(); }  \
	};

	DEFINE_STATIC_SHADER( eStage::Compute, computeShader )
	DEFINE_STATIC_SHADER( eStage::Vertex, vertexShader )
	DEFINE_STATIC_SHADER( eStage::Geometry, geometryShader )
	DEFINE_STATIC_SHADER( eStage::Pixel, pixelShader )

#undef DEFINE_STATIC_SHADER

	// Dynamic shaders are the most complicated
	template<eStage stage, class TStageStruct>
	struct DynamicShaderHelper
	{
		using Type = Shader<TStageStruct>;
		static constexpr bool ctorArg()
		{
			static_assert( stage == TStageStruct::shaderStage );
			return false;
		}
	};

#define DEFINE_DYNAMIC_SHADER( stage, DataClass )                   \
	template<class T>                                               \
	struct ShaderTypeHelper<T, stage, eShaderKind::Dynamic>:        \
		public DynamicShaderHelper<stage, typename T::DataClass>    \
	{ };

	DEFINE_DYNAMIC_SHADER( eStage::Compute, CsData )
	DEFINE_DYNAMIC_SHADER( eStage::Vertex, VsData )
	DEFINE_DYNAMIC_SHADER( eStage::Geometry, GsData )
	DEFINE_DYNAMIC_SHADER( eStage::Pixel, PsData )

#undef DEFINE_DYNAMIC_SHADER
}