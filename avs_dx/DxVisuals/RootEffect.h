#pragma once
#include "Effects/Common/EffectListBase.h"
#include "Resources/RenderTargets.h"
#include "Resources/EffectsState.h"
#include "Render/StateShaders.h"

class RootEffect : public EffectListBase
{
public:
	RootEffect( AvsState* pState ) : EffectListBase( pState ){ }

	DECLARE_EFFECT()

	HRESULT renderRoot( bool isBeat, RenderTargets& rt, bool rebindResources );

	HRESULT clearRenders() override;

	ID3D11Buffer* stateBuffer() const { return m_state.buffer(); }

private:
	EffectsState m_state;
	StateShaders m_stateShaders;

	HRESULT buildState();
};