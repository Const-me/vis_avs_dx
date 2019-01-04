#pragma once
#include "ShaderBase.h"
#include "../Hlsl/ShaderTemplate.h"
#include <Utils/resizeHandler.h>

class Binder;

// A shader that's instantiated from a template. Source data is what defines the macros.
// Source needs to be copyable, needs to have operator==, and needs to have HRESULT defines( Hlsl::Defines &def ); method.
template<class TSourceData>
class Shader: public ShaderBase<TSourceData::shaderStage>
{
public:

	using tStageData = TSourceData;

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
		if( hr.combine( m_sourceData.updateBindings( binder ) ) )
			return hr;
		if( hr.combine( m_sourceData.updateAvs( avs ) ) )
			return hr;
		if( hr.combine( m_sourceData.updateDx( dx ) ) )
			return hr;
		if( hr.value() )
			__super::setUpdated();
		if( !__super::hasShader() )
			hr.combine( true );
		return hr;
	}

	HRESULT compile( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		if( __super::isFailed() )
			return S_FALSE;

		// Generate preprocessor macro values, from the current copy of the state
		Hlsl::Defines def;
		def.set( "STATE_OFFSET", stateOffset * 4 );
		__if_exists( TSourceData::defines )
		{
			CHECK( m_sourceData.defines( def ) );
		}

		const ShaderTemplate& tmpl = *m_sourceData.shaderTemplate();
		std::vector<uint8_t> dxbc;
		const HRESULT hr = __super::compile( tmpl.name, tmpl.hlsl, inc, def, tmpl.usesBeat, dxbc );
		if( FAILED( hr ) )
			return hr;
		if( S_FALSE == hr )
			return S_FALSE;

		// Invoke optional compiledShader method. Some shaders need that to create input layouts.
		__if_exists( TSourceData::compiledShader )
		{
			CHECK( m_sourceData.compiledShader( ( const std::vector<uint8_t>& )dxbc ) );
		}

		return S_OK;
	}

	TSourceData& data() { return m_sourceData; }

	const TSourceData& data() const { return m_sourceData; }

private:

	TSourceData m_sourceData;
};