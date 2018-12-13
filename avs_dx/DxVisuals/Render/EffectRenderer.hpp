#pragma once
#include "Shader.hpp"

template<eStage stage>
class StaticShader
{
	const ShaderPtr<stage> m_shader;

public:
	StaticShader( IUnknown* pUnk ) : m_shader( ( IShader<stage>* )pUnk )
	{
		if( nullptr == pUnk )
			logError( "Static shader constructed without the shader" );
	}

	void bind() const
	{
		bindShader<stage>( m_shader );
	}
};

template<eStage stage>
struct NoShader
{
	NoShader( bool unused ) { }

	void bind() const
	{
		bindShader<stage>( nullptr );
	}
};

#include "EffectRenderer.inl"

template<class FxDef>
class EffectRenderer
{
public:
	static constexpr std::array<eShaderKind, 4> shaderKinds = { computeShaderKind<FxDef>(), vertexShaderKind<FxDef>(), geometryShaderKind<FxDef>(), pixelShaderKind<FxDef>() };
	using tAvxState = typename FxDef::AvsState;

	EffectRenderer() :
		m_shaders{ tHelper<eStage::Compute>::ctorArg(), tHelper<eStage::Vertex>::ctorArg(), tHelper<eStage::Geometry>::ctorArg(), tHelper<eStage::Pixel>::ctorArg() }
	{ }

	bool updateDynamics( const tAvxState& ass, int stateOffset )
	{
		bool res = false;
		forEachDynStage( [ =, &res ]( auto p )
		{
			const bool r = p.update( ass, stateOffset );
			res = res || r;
		} );
		return res;
	}

	bool updateBindings( Binder& binder )
	{
		// Resource bindings
		bool res = false;
		forEachDynStage( [ =, &res ]( auto p )
		{
			const bool changedBindings = p.updateBindings( binder );
			res = res || changedBindings;
		} );
		return res;
	}

	void bindShaders()
	{
		forEachStage( []( auto p ) { p.bind(); } );
	}

private:
	template<eStage stage>
	using tHelper = ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>;
	template<eStage stage>
	using tShader = typename ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>::Type;
	using tTuple = std::tuple<tShader<eStage::Compute>, tShader<eStage::Vertex>, tShader<eStage::Geometry>, tShader<eStage::Pixel>>;
	tTuple m_shaders;

	template<class Fn>
	inline void forEachStage( Fn fn )
	{
		fn( std::get<0>( m_shaders ) );
		fn( std::get<1>( m_shaders ) );
		fn( std::get<2>( m_shaders ) );
		fn( std::get<3>( m_shaders ) );
	}

	template<class Fn>
	inline void forEachDynStage( Fn fn )
	{
		if constexpr( shaderKinds[ 0 ] == eShaderKind::Dynamic )
			fn( std::get<0>( m_shaders ) );
		if constexpr( shaderKinds[ 1 ] == eShaderKind::Dynamic )
			fn( std::get<1>( m_shaders ) );
		if constexpr( shaderKinds[ 2 ] == eShaderKind::Dynamic )
			fn( std::get<2>( m_shaders ) );
		if constexpr( shaderKinds[ 3 ] == eShaderKind::Dynamic )
			fn( std::get<3>( m_shaders ) );
	}
};