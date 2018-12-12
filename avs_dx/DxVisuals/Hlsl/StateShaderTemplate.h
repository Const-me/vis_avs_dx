#pragma once

// Template of an HLSL shader. To convert it into the real one, it needs to be compiled with some effect-specific preprocessor defines.
struct StateShaderTemplate
{
	// Name is only used for log messages.
	const char* const name;

	// HLSL source code of the main() function, excluding the signature but including the { }
	const CStringA hlslMain;

	// HLSL source code of the global sections
	const std::vector<CStringA>& globals;

	// Some part of the shader uses IS_BEAT macro. The runtime will compile two different versions of the shader.
	// This way it's slightly faster than runtime branching on `beat` from FrameGlobalData cbuffer
	const bool hasBeat;

	StateShaderTemplate( const char *n, const CStringA& src, bool beat, const std::vector<CStringA> &glob ) :
		name( n ),
		hlslMain( src ),
		hasBeat( beat ),
		globals( glob )
	{ }
};