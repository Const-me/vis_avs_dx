#pragma once
#include "ShaderBase.h"
#include "../Hlsl/ShaderTemplate.h"
#include <Utils/resizeHandler.h>

class Binder;

// A shader that's instantiated from a template. Source data is what defines the macros.
// Source needs to be copyable, needs to have operator==, and needs to have HRESULT defines( Hlsl::Defines &def ); method.
template<class TSourceData>
class Shader : public ShaderBase<TSourceData::shaderStage>
{
public:

	using tStageData = TSourceData;

	Shader( const CAtlMap<CStringA, CStringA>& includes ) :
		ShaderBase<TSourceData::shaderStage>( includes )
	{ }

	Shader() :
		ShaderBase<TSourceData::shaderStage>( Hlsl::includes() )
	{ }

	Shader( const Shader& ) :
		ShaderBase<TSourceData::shaderStage>( Hlsl::includes() )
	{
		// eastl::variant requires objects to be copyable, for no reason.
		__debugbreak();
	}
	Shader( bool unused ) { }

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

	HRESULT compile( UINT stateOffset )
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
		const HRESULT hr = __super::compile( tmpl.name, tmpl.hlsl, def, tmpl.usesBeat, &onCompiledShader, &m_sourceData );
		if( FAILED( hr ) )
			return hr;
		if( S_FALSE == hr )
			return S_FALSE;
		return S_OK;
	}

	TSourceData& data() { return m_sourceData; }

	const TSourceData& data() const { return m_sourceData; }

private:

	static void onCompiledShader( void* pThis, const vector<uint8_t>& dxbc )
	{
		__if_exists( TSourceData::compiledShader )
		{
			TSourceData* ps = (TSourceData*)pThis;
			ps->compiledShader( dxbc );
		}
	}

	TSourceData m_sourceData;
};