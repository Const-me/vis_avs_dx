#pragma once
#include "../Effects/EffectBase.h"
#include "EffectsState.h"

class Renderer
{
	// Size of visualization in pixels.
	CSize m_pixelSize;

	// Compute shaders to update states of all effects
	CComPtr<ID3D11ComputeShader> m_updateState;
	// If no effect passed fnUpdateOnBeat function, this will be the same object as m_updateState. Otherwise it will be slightly different compute shader.
	CComPtr<ID3D11ComputeShader> m_updateStateBeat;

	EffectsState m_state;

public:

	void updateState( bool beat );
};