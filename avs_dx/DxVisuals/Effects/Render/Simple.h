#pragma once
#include "../EffectBase.h"

struct Simple: public EffectBase
{
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;

	struct AvsState
	{
		int effect;
		int num_colors;
		int colors[ 16 ];
		int color_pos;
	};
};