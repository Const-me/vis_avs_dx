#pragma once
#include "../iEffect.h"
class EffectStateBuilder;
class Binder;
class RenderTargets;

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

	// Effects will return true if user has changed their settings in a way that deprecates GPU state buffer data and/or state shaders
	virtual bool shouldRebuildState() { return false; }

	virtual HRESULT buildState( int stateBufferOffset, int& thisSize, CStringA& hlsl, bool& useBeat, CAtlMap<CStringA, int>& globals ){ return S_FALSE; }

	// If user has changed something with the GUI controls, this method will recompile shaders / update GPU resources accordingly.
	virtual HRESULT updateParameters( Binder& binder ) { return S_FALSE; }

	virtual HRESULT render( RenderTargets& rt ) = 0;

protected:

	int m_stateOffset;

	int stateOffset() const { return m_stateOffset; }

	void setStateOffset( int off ) { m_stateOffset = off; }
};