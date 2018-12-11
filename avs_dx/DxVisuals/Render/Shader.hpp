#pragma once
#include "Stage.h"
#include "../Hlsl/Defines.h"
#include "../Hlsl/Compiler.h"
#include "../Resources/createShaders.hpp"

struct ShaderTemplate
{
	const char* const name;
	const CStringA& hlsl;
	constexpr ShaderTemplate( const char *n, const CStringA& src ) : name( n ), hlsl( src ) { }
};

// A shader that's instantiated from a template. Source data is arbitrary type, needs to be copyable and need to have operator==.
template<eStage stage, class TSourceData>
class Shader
{
public:
	Shader( const ShaderTemplate& st ) :shaderTemplate( st ) { }

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
		if( FAILED( hr ) )
		{
			errors.Trim();
			logError( "Error compiling %s shader %s: %s", shaderName<stage>(), shaderTemplate.name, errors.operator const char*( ) );
			return hr;
		}
		if( errors.GetLength() > 0 )
		{
			errors.Trim();
			logWarning( "Compilation warnings for %s shader %s: %s", shaderName<stage>(), shaderTemplate.name, errors.operator const char*( ) );
		}

		// Upload DXBC to GPU
		CHECK( createShader( dxbc, result ) );

		// Save the new state
		m_source = sourceData;
		return S_OK;
	}

	operator IShader<stage>* ( ) const
	{
		return result;
	}

private:
	const ShaderTemplate& shaderTemplate;
	TSourceData m_source;
	ShaderPtr<stage> result;
};