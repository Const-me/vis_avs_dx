#pragma once
#include "EffectListBase.h"
#include "../Resources/RenderTarget.h"

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
	HRESULT renderEffects();
};