#pragma once
#include "iTransition.h"
#include <Utils/resizeHandler.h>
#include "Resources/RenderTargets.h"
#include <Effects/Common/EffectProfiler.h>

class Transition:
	public ResizeHandler,
	public iTransition
{
	RenderTargets m_targets1, m_targets2;

	void onRenderSizeChanged() override
	{
		m_targets1.destroy();
		m_targets2.destroy();
	}

public:
	Transition();

	~Transition() override;

	HRESULT renderSingle( uint16_t visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *pRBase ) override;

	HRESULT renderTransition( uint16_t visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *e1, const C_RBASE *e2, int trans, float sintrans ) override;

private:

	HRESULT prepare( uint16_t visdata[ 2 ][ 2 ][ 576 ], int isBeat );
	CD3D11_VIEWPORT m_viewport;

	EffectProfiler m_prepared, m_rendered;
};

Transition* getTransitionInstance();

bool destroyTransitionInstance();