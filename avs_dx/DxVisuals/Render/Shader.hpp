#pragma once
#include "Stage.h"
#include "../Hlsl/ShaderTemplate.h"
#include "../Hlsl/Defines.h"
#include "../Hlsl/compile.h"
#include "../Resources/createShader.hpp"

// A shader that's instantiated from a template. Source data is what defines the macros.
// Source needs to be copyable, needs to have operator==, and needs to have HRESULT defines( Hlsl::Defines &def ); method.
template<eStage stage, class TSourceData>
class Shader
{
public:
	using tStageData = TSourceData;
	Shader() = default;
	Shader( bool unused ) { }

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
		return hr;
	}

	HRESULT compile( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		// Drop the old shader
		result = nullptr;

		// Generate preprocessor macro values, from the current copy of the state
		Hlsl::Defines def;
		def.set( "STATE_OFFSET", stateOffset * 4 );
		__if_exists( TSourceData::defines )
		{
			CHECK( m_sourceData.defines( def ) );
		}

		const ShaderTemplate& tmpl = *m_sourceData.shaderTemplate();

		// Compile HLSL into DXBC
		std::vector<uint8_t> dxbc;
		CHECK( Hlsl::compile( stage, tmpl.hlsl, tmpl.name, inc, def, dxbc ) );

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, result ) );

		return S_OK;
	}

	TSourceData& data() { return m_sourceData; }
	const TSourceData& data() const { return m_sourceData; }

	void bind() const
	{
		if( nullptr == result )
			logWarning( "%s shader %s: binding shader that wasn't compiled", Hlsl::targetName( stage ), m_sourceData.shaderTemplate()->name );
		bindShader<stage>( result );
	}

	// Check if this shader was compiled successfully.
	bool hasShader() const
	{
		return nullptr != result;
	}

	// Drop the compiled shader
	void dropShader()
	{
		result = nullptr;
	}

private:
	TSourceData m_sourceData;
	ShaderPtr<stage> result;
};