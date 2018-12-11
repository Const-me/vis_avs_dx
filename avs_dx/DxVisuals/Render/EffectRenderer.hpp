#pragma once
#include "../Hlsl/Shader.hpp"

template<eStage stage>
class StaticShader
{
	const ShaderPtr<stage> m_shader;
public:
	StaticShader( IShader<stage>* i ) : m_shader( i ) { }

	template<class TSourceData>
	constexpr bool needsCompiling( const TSourceData& src ) const
	{
		return false;
	}
	template<class TSourceData>
	constexpr HRESULT compile( const TSourceData& sourceData )
	{
		return S_FALSE;
	}
	operator IShader<stage>* ( ) const
	{
		return m_shader;
	}
};

template<eStage stage>
struct NoShader
{
	template<class TSourceData>
	constexpr bool needsCompiling( const TSourceData& src ) const
	{
		return false;
	}
	template<class TSourceData>
	constexpr HRESULT compile( const TSourceData& sourceData )
	{
		return S_FALSE;
	}
	operator IShader<stage>* ( ) const
	{
		return nullptr;
	}
};

namespace
{
	// Horrible metaprogramming BS below. I wish I had compile-time reflection instead.
	enum struct eShaderKind : uint8_t
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
}

template<class FxDef>
class EffectRenderer
{
public:
	static constexpr std::array<eShaderKind, 4> shaderKinds = { computeShaderKind<FxDef>(), vertexShaderKind<FxDef>(), geometryShaderKind<FxDef>(), pixelShaderKind<FxDef>() };
};