#pragma once
#include "../iEffect.h"
#include "../Render/EffectStateShader.hpp"
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

	// Effects will return S_OK if user has changed their settings in a way that deprecates GPU state buffer data and/or state shaders.
	// S_FALSE means the previous state is fine.
	virtual HRESULT shouldRebuildState() { return S_FALSE; }

	virtual HRESULT buildState( EffectStateShader& ess ){ return S_FALSE; }

	virtual HRESULT initializedState() { return S_FALSE; }

	// If user has changed something with the GUI controls, this method will recompile shaders / update GPU resources accordingly.
	virtual HRESULT updateParameters( Binder& binder ) { return S_FALSE; }

	virtual HRESULT render( RenderTargets& rt ) = 0;

	void setStateOffset( UINT off ) { m_stateOffset = off; }

protected:

	UINT m_stateOffset;

	UINT stateOffset() const { return m_stateOffset; }
};