#pragma once
#include "Stage.h"
#include "../Hlsl/ShaderTemplate.h"
#include "../Hlsl/Defines.h"
#include "../Hlsl/compile.h"
#include "../Resources/createShader.hpp"
#include <Utils/resizeHandler.h>

class Binder;

// A shader that's instantiated from a template. Source data is what defines the macros.
// Source needs to be copyable, needs to have operator==, and needs to have HRESULT defines( Hlsl::Defines &def ); method.
template<class TSourceData>
class Shader: public iResizeHandler
{
	enum struct eShaderState : uint8_t
	{
		Constructed,
		Updated,
		Failed,
		Good,
	};
	eShaderState m_state = eShaderState::Constructed;

public:
	using tStageData = TSourceData;
	static constexpr eStage shaderStage = TSourceData::shaderStage;

	Shader() = default;
	Shader( bool unused ) { }
	~Shader()
	{
		unsubscribeHandler( this );
	}

	template<class TAvsState, class TDxEffectState>
	HRESULT update( Binder& binder, const TAvsState& avs, const TDxEffectState& dx )
	{
		BoolHr hr;
		__if_exists( TSourceData::updateBindings )
		{
			if( hr.combine( m_sourceData.updateBindings( binder ) ) )
				return hr;
		}
		__if_exists( TSourceData::updateAvs )
		{
			if( hr.combine( m_sourceData.updateAvs( avs ) ) )
				return hr;
		}
		__if_exists( TSourceData::updateDx )
		{
			if( hr.combine( m_sourceData.updateDx( dx ) ) )
				return hr;
		}
		if( hr.value() )
			m_state = eShaderState::Updated;
		if( !hasShader() )
			hr.combine( true );
		return hr;
	}

	HRESULT compile( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		if( eShaderState::Failed == m_state )
			return S_FALSE;
		dropShader();
		m_state = eShaderState::Failed;

		// Generate preprocessor macro values, from the current copy of the state
		Hlsl::Defines def;
		def.set( "STATE_OFFSET", stateOffset * 4 );
		__if_exists( TSourceData::defines )
		{
			CHECK( m_sourceData.defines( def ) );
		}

		const ShaderTemplate& tmpl = *m_sourceData.shaderTemplate();

		if( tmpl.hlsl.Find( "AVS_RENDER_SIZE" ) >= 0 )
		{
			def.set( "AVS_RENDER_SIZE", getRenderSizeString() );
			subscribeHandler( this );
		}
		else
			unsubscribeHandler( this );

		if( tmpl.usesBeat )
			def.set( "IS_BEAT", "0" );

		// Compile HLSL into DXBC
		std::vector<uint8_t> dxbc;
		CHECK( Hlsl::compile( shaderStage, tmpl.hlsl, tmpl.name, inc, def, dxbc ) );

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, shader ) );

		if( tmpl.usesBeat )
		{
			def.reset( "IS_BEAT", "1" );
			CHECK( Hlsl::compile( shaderStage, tmpl.hlsl, tmpl.name, inc, def, dxbc ) );
			CHECK( createShader( dxbc, beatShader ) );
		}
		else
			beatShader = shader;

		// Invoke optional compiledShader method. Some shaders need that to create input layouts.
		__if_exists( TSourceData::compiledShader )
		{
			CHECK( m_sourceData.compiledShader( ( const std::vector<uint8_t>& )dxbc ) );
		}

		m_state = eShaderState::Good;
		return S_OK;
	}

	TSourceData& data() { return m_sourceData; }
	const TSourceData& data() const { return m_sourceData; }

	bool bind( bool isBeat ) const
	{
		const ShaderPtr<shaderStage>& s = isBeat ? beatShader : shader;
		// if( nullptr == s )
		//	logWarning( "%s shader %s: binding shader that wasn't compiled", Hlsl::targetName( shaderStage ), m_sourceData.shaderTemplate()->name );
		if( nullptr == s )
			return false;
		bindShader<shaderStage>( s );
		return true;
	}

	// Check if this shader was compiled successfully.
	bool hasShader() const
	{
		return m_state == eShaderState::Good;
	}

	// Drop the compiled shader
	void dropShader()
	{
		shader = beatShader = nullptr;
		m_state = eShaderState::Updated;
	}

	IShader<shaderStage> *ptr( bool isBeat ) const
	{
		return isBeat ? beatShader : shader;
	}

private:
	TSourceData m_sourceData;
	ShaderPtr<shaderStage> shader, beatShader;

	// iResizeHandler
	void onRenderSizeChanged() override
	{
		dropShader();
	}
};