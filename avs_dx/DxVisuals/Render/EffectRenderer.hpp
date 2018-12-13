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

	bool updateBindings( Binder& binder )
	{
		bool res = false;
		forEachDynStage( [ & ]( auto p )
		{
			const bool changedBindings = p.updateBindings( binder );
			res = res || changedBindings;
		} );
		return res;
	}

	HRESULT updateValues( const tAvxState& ass, UINT stateOffset )
	{
		HRESULT res = S_FALSE;
		forEachDynStage( [ =, &res ]( auto p )
		{
			using tData = decltype( p );
			__if_not_exists( tData::update )
			{
				return S_FALSE;
			}

			__if_exists( tData::update )
			{
				const HRESULT hr = p.update( ass, stateOffset );
				if( FAILED( hr ) )
				{
					logError( hr, "Update failed" );
					return;
				}
				if( S_FALSE != hr )
					res = S_OK;
			}
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