#pragma once
#include "iTransition.h"
#include <Utils/resizeHandler.h>
#include "Resources/RenderTargets.h"

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
	~Transition() override;

	HRESULT renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e ) override;

	HRESULT renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e1, iRootEffect &e2, int trans, float sintrans ) override;

private:

	HRESULT prepare( char visdata[ 2 ][ 2 ][ 576 ], int isBeat );
};