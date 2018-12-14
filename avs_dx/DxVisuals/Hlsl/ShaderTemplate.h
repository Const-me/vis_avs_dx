#pragma once

// Template of an HLSL shader. To convert it into the real one, it needs to be compiled with some effect-specific preprocessor defines.
struct ShaderTemplate
{
	// Name is only used for log messages.
	const char* const name;

	// HLSL source code of the template
	CStringA hlsl;

	constexpr ShaderTemplate( const char *n, const CStringA& src ) : name( n ), hlsl( src ) { }
	ShaderTemplate( const ShaderTemplate& that ) : name( that.name ), hlsl( that.hlsl ) { }
};