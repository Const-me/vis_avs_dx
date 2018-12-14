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
	Shader() = default;
	Shader( bool unused ) { }

	bool updateBindings( Binder& binder )
	{
		return m_source.updateBindings( binder );
	}

	template<class tAvxState>
	HRESULT updateValues( const tAvxState& ass )
	{
		const HRESULT h1 = m_source.update( ass );
		// return S_OK when update returnned S_OK, or when the shader is nullptr.
		return hr_or( h1, result ? S_FALSE : S_OK );
	}

	HRESULT compile( const CAtlMap<CStringA, CStringA>& inc, UINT stateOffset )
	{
		// Drop the old shader
		result = nullptr;

		// Generate preprocessor macro values, from the current copy of the state
		Hlsl::Defines def;
		def.set( "STATE_OFFSET", stateOffset * 4 );
		CHECK( m_source.defines( def ) );

		const ShaderTemplate& tmpl = *m_source.shaderTemplate();

		// Compile HLSL into DXBC
		std::vector<uint8_t> dxbc;
		CHECK( Hlsl::compile( stage, tmpl.hlsl, tmpl.name, inc, def, dxbc ) );

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, result ) );

		return S_OK;
	}

	void bind() const
	{
		if( nullptr == result )
			logWarning( "%s shader %s: binding shader that wasn't compiled", Hlsl::targetName( stage ), m_source.shaderTemplate()->name );
		bindShader<stage>( result );
	}
	const TSourceData& data() const { return m_source; }

	void dropShader()
	{
		result = nullptr;
	}

private:
	TSourceData m_source;
	ShaderPtr<stage> result;
};