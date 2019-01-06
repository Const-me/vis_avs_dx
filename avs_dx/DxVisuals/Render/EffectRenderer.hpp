#pragma once
#include "Shader.hpp"
#include <Resources/createShader.hpp>
#include "SimpleShaderTypes.hpp"
#include "EffectRenderer.inl"
#include <EASTL/tuple.h>

// A template class that "assembles" effects from the state + shaders, updates per-stage data, compiles, updates, and binds these shaders.
template<class FxDef>
class EffectRenderer
{
public:
	static constexpr array<eShaderKind, 4> shaderKinds = { computeShaderKind<FxDef>(), vertexShaderKind<FxDef>(), geometryShaderKind<FxDef>(), pixelShaderKind<FxDef>() };
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
		BoolHr hr = forEachDynStage( [ & ]( auto& p ) { return p.update( binder, avs, dx ); } );
		hr.combine( forEachBinStage( [ & ]( auto& p ) { return p.compile(); } ) );
		return hr;
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
		return eastl::get<0>( m_shaders ).data();
	}
	// Vertex shader source data
	decltype( auto ) vertex()
	{
		return eastl::get<1>( m_shaders ).data();
	}
	// Geometry shader source data
	decltype( auto ) geometry()
	{
		return eastl::get<2>( m_shaders ).data();
	}
	// Pixel shader source data
	decltype( auto ) pixel()
	{
		return eastl::get<3>( m_shaders ).data();
	}

	template<eStage stage>
	HRESULT compileShader( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		constexpr uint8_t i = (uint8_t)stage;
		static_assert( shaderKinds[ i ] == eShaderKind::Dynamic );
		return eastl::get<i>( m_shaders ).compile( inc, stateOffset );
	}

	// Bind shaders for all 4 stages. If some are not defined in the effect structure, these shaders will be unbound.
	bool bindShaders( bool isBeat )
	{
		bool result = true;
		forEachStage( [ &result, isBeat ]( auto& p )
		{
			const bool r = p.bind( isBeat );
			result = result && r;
		} );
		return result;
	}

private:
	template<eStage stage>
	using tHelper = ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>;
	template<eStage stage>
	using tShader = typename ShaderTypeHelper<FxDef, stage, shaderKinds[ (uint8_t)stage ]>::Type;
	using tTuple = eastl::tuple<tShader<eStage::Compute>, tShader<eStage::Vertex>, tShader<eStage::Geometry>, tShader<eStage::Pixel>>;
	tTuple m_shaders;

	template<class Fn>
	inline void forEachStage( Fn fn )
	{
		fn( eastl::get<0>( m_shaders ) );
		fn( eastl::get<1>( m_shaders ) );
		fn( eastl::get<2>( m_shaders ) );
		fn( eastl::get<3>( m_shaders ) );
	}

	template<class Fn>
	inline HRESULT forEachDynStage( Fn fn )
	{
		BoolHr hr;
		if constexpr( shaderKinds[ 0 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( eastl::get<0>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 1 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( eastl::get<1>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 2 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( eastl::get<2>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 3 ] == eShaderKind::Dynamic )
			if( hr.combine( fn( eastl::get<3>( m_shaders ) ) ) )
				return hr;
		return hr;
	}

	template<class Fn>
	inline HRESULT forEachBinStage( Fn fn )
	{
		BoolHr hr;
		if constexpr( shaderKinds[ 0 ] == eShaderKind::Binary )
			if( hr.combine( fn( eastl::get<0>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 1 ] == eShaderKind::Binary )
			if( hr.combine( fn( eastl::get<1>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 2 ] == eShaderKind::Binary )
			if( hr.combine( fn( eastl::get<2>( m_shaders ) ) ) )
				return hr;
		if constexpr( shaderKinds[ 3 ] == eShaderKind::Binary )
			if( hr.combine( fn( eastl::get<3>( m_shaders ) ) ) )
				return hr;
		return hr;
	}
};