#pragma once
#include "Shader.hpp"

template<eStage stage>
class StaticShader
{
	const ShaderPtr<stage> m_shader;

public:
	StaticShader( IUnknown* pUnk ) : m_shader( ( IShader<stage>* )pUnk ) { }
	operator IShader<stage>* ( ) const
	{
		return m_shader;
	}
};

template<eStage stage>
struct NoShader
{
	NoShader( bool unused ) { }
	operator IShader<stage>* ( ) const
	{
		return nullptr;
	}
};

#include "EffectRenderer.inl"

template<class FxDef>
class EffectRenderer
{
public:
	static constexpr std::array<eShaderKind, 4> shaderKinds = { computeShaderKind<FxDef>(), vertexShaderKind<FxDef>(), geometryShaderKind<FxDef>(), pixelShaderKind<FxDef>() };

	EffectRenderer() :
		m_shaders{ tHelper<eStage::Compute>::ctorArg(), tHelper<eStage::Vertex>::ctorArg(), tHelper<eStage::Geometry>::ctorArg(), tHelper<eStage::Pixel>::ctorArg() }
	{ }

private:
	template<eStage stage>
	using tHelper = ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>;
	template<eStage stage>
	using tShader = typename ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>::Type;
	using tTuple = std::tuple<tShader<eStage::Compute>, tShader<eStage::Vertex>, tShader<eStage::Geometry>, tShader<eStage::Pixel>>;
	tTuple m_shaders;
};