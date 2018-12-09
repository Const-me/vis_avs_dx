#pragma once

struct Resource {};
typedef Resource *ResourceHandle;

class EffectsResources
{
public:
	// Declare a persistent structured buffer resource, e.g. for extra compute pass.
	HRESULT structuredBuffer( size_t elements, size_t sizeElement, const char* hlslName, ResourceHandle& buffer );

	// The effect will read from the frame texture. Some effects only write there, but some want to read as well: movements, custom blending, blur, etc.
	// For such effects, renderer will create a new instance of the frame buffer, and bind the current one into the Texture2D resource.
	HRESULT readFrame( const char* hlslName, ResourceHandle& texture );
};