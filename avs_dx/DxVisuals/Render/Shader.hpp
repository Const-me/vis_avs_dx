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
	Shader( const ShaderTemplate* st ) : shaderTemplate( *st ) { }

	bool updateBindings( Binder& binder )
	{
		return m_source.updateBindings( binder );
	}

	template<class tAvxState>
	HRESULT updateValues( const tAvxState& ass )
	{
		return m_source.update( ass );
	}

	HRESULT compile( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		// Drop the old shader
		result = nullptr;

		// Generate preprocessor macro values, from the current copy of the state
		Hlsl::Defines def;
		def.set( "STATE_OFFSET", stateOffset * 4 );
		CHECK( m_source.defines( def ) );

		// Compile HLSL into DXBC
		std::vector<uint8_t> dxbc;
		CHECK( Hlsl::compile( stage, shaderTemplate.hlsl, shaderTemplate.name, inc, def, dxbc ) );

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, result ) );

		return S_OK;
	}

	void bind() const
	{
		if( nullptr == result )
			logWarning( "%s shader %s: binding shader that wasn't compiled", Hlsl::targetName( stage ), shaderTemplate.name );
		bindShader<stage>( result );
	}
	const TSourceData& data() const { return m_source; }

private:
	const ShaderTemplate& shaderTemplate;
	TSourceData m_source;
	ShaderPtr<stage> result;
};