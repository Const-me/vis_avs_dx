#pragma once
#include "../Render/EffectStateShader.hpp"
class Binder;
class RenderTargets;
class RootEffect;

// All effects must inherit from this abstract class.
class EffectBase
{
public:
	virtual ~EffectBase() { }

	struct Metadata
	{
		const char* const name;
		const bool isList;
		Metadata() = delete;
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
	// This method also reserves /updates resource slots used by this effect.
	virtual HRESULT updateParameters( Binder& binder ) { return S_FALSE; }

	// After updateParameters() returns S_OK, this method will be called before rendering, to bind all preset specific resources to the slots declared by the binder.
	virtual void bindResources() { }

	virtual HRESULT render( bool isBeat, RenderTargets& rt ) = 0;

	virtual void setStateOffset( UINT off ) { m_stateOffset = off; }

	void setOwner( RootEffect *p ) { m_pRoot = p; }

	// Called by EffectListBase each time after render() returns S_OK. The current implementation does GPU profiling measures, unless disabled by GPU_PROFILE=0 macro.
	virtual HRESULT completedRendering() { return S_OK; }

protected:

	UINT m_stateOffset;

	// State offset, in 4-byte items
	UINT stateOffset() const { return m_stateOffset; }

	ID3D11Buffer* stateBuffer() const;

private:

	RootEffect *m_pRoot = nullptr;
};

static constexpr HRESULT S_CLEAR_BEAT = 2;
static constexpr HRESULT S_SET_BEAT = 3;