#pragma once

// Template of an HLSL state compute shader. State shaders are assembled in runtime form per-effect pieces. This way we only need a single ID3D11DeviceContext::Dispatch() call to update state of all effects.
struct StateShaderTemplate
{
	// Name is only used for log messages.
	const char* const name;

	// HLSL source code of the main() function, excluding the signature but including the { }
	CStringA hlslMain;

	// HLSL source code of the global sections
	const vector<CStringA> *globals = nullptr;

	// Some part of the shader uses IS_BEAT macro. The runtime will compile two different versions of the shader.
	// This way it's slightly faster than runtime branching on `beat` from FrameGlobalData cbuffer
	bool hasBeat = false;

	// True if the template uses random numbers
	bool hasRandomNumbers = false;

	StateShaderTemplate( const char *n ) : name( n ) { }

	StateShaderTemplate( const char *n, const CStringA& src, bool beat, const vector<CStringA> &glob ) :
		name( n ),
		hlslMain( src ),
		hasBeat( beat ),
		globals( &glob )
	{ }
};