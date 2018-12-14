#pragma once
#include "EffectListBase.h"
#include "../Resources/RenderTargets.h"
#include "../Resources/EffectsState.h"
#include "../Render/StateShaders.h"

class RootEffect : public EffectListBase
{
	CSize m_renderSize;
	RenderTargets m_targets;

public:
	RootEffect( AvsState* pState ) : EffectListBase( pState ), m_renderSize{ 0, 0 }{ }
	const Metadata& metadata() override;

	HRESULT renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat ) override;

private:
	EffectsState m_state;
	StateShaders m_stateShaders;

	HRESULT buildState();

	HRESULT renderEffects( bool isBeat );
};