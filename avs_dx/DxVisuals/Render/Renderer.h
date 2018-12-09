#pragma once
#include "../Effects/EffectBase.h"
#include "EffectsState.h"

class Renderer
{
	// Size of visualization in pixels.
	CSize m_pixelSize;

	// Compute shaders to update the state of all running effects
	CComPtr<ID3D11ComputeShader> m_updateState;
	// If no effect passed fnUpdateOnBeat function, this will be the same as m_updateState. Otherwise it will be slightly different compute shader.
	CComPtr<ID3D11ComputeShader> m_updateStateBeat;

	// Merged state buffer for all effects. It's merged so we have a single compute shaders updating all of them: draw calls are relatively expensive.
	EffectsState m_state;

	std::vector<EffectBase*> m_effects;

public:
	void add( EffectBase& e )
	{
		m_effects.push_back( &e );
	}
};