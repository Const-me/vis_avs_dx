#pragma once
#include "../EffectImpl.hpp"

class Simple: public EffectBase
{
public:
	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;

	DECLARE_EFFECT( Simple );

	struct AvsState
	{
		int effect;
		int num_colors;
		int colors[ 16 ];
		int color_pos;
	};
};