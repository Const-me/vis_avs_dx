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

	bool needsCompiling( const TSourceData& src ) const
	{
		if( !result )
			return true;
		if( !( src == m_source ) )
			return true;
		return false;
	}

	HRESULT compile( const TSourceData& sourceData )
	{
		// Drop the old shader
		result = nullptr;

		// Generate preprocessor macro values
		Hlsl::Defines def;
		CHECK( sourceData.defines( def ) );

		// Compile HLSL into DXBC
		std::vector<uint8_t> dxbc;
		CStringA errors;
		const HRESULT hr = Hlsl::compile( stage, shaderTemplate.hlsl, dxbc, errors, &def );

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, result ) );

		// Save the new state
		m_source = sourceData;
		return S_OK;
	}

	void bind() const
	{
		if( nullptr == result )
			logWarning( "%s shader %s: binding shader that wasn't compiled", Hlsl::targetName( stage ), shaderTemplate.name );
		bindShader<stage>( result );
	}

private:
	const ShaderTemplate& shaderTemplate;
	TSourceData m_source;
	ShaderPtr<stage> result;
};