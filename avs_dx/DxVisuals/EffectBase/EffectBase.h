#pragma once
#include "../iEffect.h"
class EffectStateBuilder;

class EffectBase: public iEffect
{
public:
	struct Metadata
	{
		const char* const name;
		const bool isList;
		Metadata( const char* n, bool il ) : name( n ), isList( il ) { }
	};

	// Get the compile-time metadata for this effect instance.
	virtual const Metadata& metadata() = 0;

	// Effects will return true if user has changed their settings in a way that deprecates GPU state buffer data.
	virtual bool updateState() { return false; }

	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;

	// If user has changed something with the GUI controls, this method will recompile shaders / update GPU resources accordingly.
	virtual HRESULT updateParameters() { return S_FALSE; }
};