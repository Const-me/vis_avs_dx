#pragma once
#include "../Hlsl/FunctionBuilder.h"
#include "../Hlsl/ShaderBuilder.h"

struct Resource
{
	enum struct eKind
	{
		// ConstBuffer,
		Texture2D,
		StructBuffer
	};
	const eKind kind;
	const int id;
	const int slot;
};

class EffectBuilder
{
public:
	using fnBuildShaderFunc = std::function<HRESULT( int stateOffset, Hlsl::FunctionBuilder& hlsl )>;

	// Declare an effect state block that will be updated on every frame.
	HRESULT state( int stateSize, fnBuildShaderFunc &&fnUpdate );
	// Declare an effect state block that will be updated on every frame, plus some extra update code when the frame has a beat detected.
	HRESULT state( int stateSize, fnBuildShaderFunc &&fnUpdate, fnBuildShaderFunc &&fnUpdateOnBeat, bool alwaysRunUpdate = true );

	// Some effects want to consume extra variables updated by CPU, we pass that data in constant buffers. The buffer will have D3D11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE. Disabled for now.
	// HRESULT constantBuffer( size_t cb, const char* hlslName, Resource& buffer );

	// Declare a persistent structured buffer resource for extra compute pass.
	HRESULT structuredBuffer( size_t elements, size_t sizeElement, const char* hlslName, Resource& buffer );

	// The effect will read from the frame texture. Some effects only write there, but some want to read as well: movements, custom blending, blur, etc.
	// For such effects, renderer will create a new instance of the frame buffer, and bind the current one into the Texture2D resource.
	HRESULT readFrame( const char* hlslName, Resource& texture );

private:
	fnBuildShaderFunc m_stateUpdate, m_stateBeat;
	bool m_alwaysUpdateState = true;
};