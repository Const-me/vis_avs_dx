#pragma once
#include "EffectListBase.h"
#include "../Resources/RenderTarget.h"
#include "../Resources/EffectsState.h"

class RootEffect : public EffectListBase
{
	CSize m_renderSize;
	std::array<RenderTarget, 2> m_targets;
	size_t m_lastTarget = 0;

public:
	RootEffect( AvsState* pState ) : EffectListBase( pState ), m_renderSize{ 0, 0 }{ }
	const Metadata& metadata() override;

	HRESULT renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat ) override;

private:
	EffectsState m_state;
	// Compute shaders to update states of all effects
	CComPtr<ID3D11ComputeShader> m_updateState;
	// If no effect passed fnUpdateOnBeat function, this will be the same object as m_updateState. Otherwise it will be slightly different compute shader.
	CComPtr<ID3D11ComputeShader> m_updateStateBeat;

	HRESULT buildState();

	HRESULT renderEffects( bool isBeat );
};