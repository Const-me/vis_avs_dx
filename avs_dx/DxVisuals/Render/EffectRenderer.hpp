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

	const bool data()
	{
		static_assert( false, "That shader is static, it has no data" );
		return false;
	};

	void bind( bool isBeat ) const
	{
		bindShader<stage>( m_shader );
	}
};

template<eStage stage>
struct NoShader
{
	NoShader( bool unused ) { }

	const bool data()
	{
		static_assert( false, "The effect has no shader for that pipeline stage" );
		return false;
	};

	void bind( bool isBeat ) const
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
	// AVS native state
	using tAvxState = typename FxDef::AvsState;
	// DX effect's state
	using tDxState = typename FxDef::StateData;

	EffectRenderer() :
		m_shaders{ tHelper<eStage::Compute>::ctorArg(), tHelper<eStage::Vertex>::ctorArg(), tHelper<eStage::Geometry>::ctorArg(), tHelper<eStage::Pixel>::ctorArg() }
	{ }

	// If this effect has no dynamic shader stages, just return S_FALSE. Otherwise, call updateBindings(), updateAvs() and updateDx() methods on the state object, and combine the results.
	HRESULT update( Binder& binder, const tAvxState& avs, const tDxState& dx )
	{
		return forEachDynStage( [ & ]( auto& p )
		{
			const HRESULT hr = p.update( binder, avs, dx );
			if( hr != S_FALSE )
				p.dropShader();
			return hr;
		} );
	}

	HRESULT compileShaders( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		return forEachDynStage( [ & ]( auto& p )
		{
			return p.compile( inc, stateOffset );
		} );
	}

	// Compute shader source data
	decltype( auto ) compute()
	{
		return std::get<0>( m_shaders ).data();
	}
	// Vertex shader source data
	decltype( auto ) vertex()
	{
		return std::get<1>( m_shaders ).data();
	}
	// Geometry shader source data
	decltype( auto ) geometry()
	{
		return std::get<2>( m_shaders ).data();
	}
	// Pixel shader source data
	decltype( auto ) pixel()
	{
		return std::get<3>( m_shaders ).data();
	}

	void bindShaders( bool isBeat )
	{
		forEachStage( [ isBeat ]( auto& p ) { p.bind( isBeat ); } );
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
	inline HRESULT forEachDynStage( Fn fn )
	{
		BoolHr hr;
		if constexpr( shaderKinds[ 0 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( std::get<0>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 1 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( std::get<1>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 2 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( std::get<2>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 3 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( std::get<3>( m_shaders ) ) ) )
				return hr;
		return hr;
	}
};