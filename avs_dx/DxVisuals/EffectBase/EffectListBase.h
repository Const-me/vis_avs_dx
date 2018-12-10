#pragma once
#include "../Effects/EffectImpl.hpp"

class EffectListBase : public EffectBase
{
public:
	DECLARE_EFFECT( EffectListBase );

	struct T_RenderListType
	{
		C_RBASE *render;
		int effect_index;
		int has_rbase2;

		bool hasDxEffect() const
		{
			return render->dxEffect.operator bool();
		}
		EffectBase* dxEffect() const;
	};

	struct AvsState
	{
		int *thisfb;
		int l_w, l_h;
		int isroot;

		int num_renders, num_renders_alloc;
		T_RenderListType *renders;
		int inblendval, outblendval;
		int bufferin, bufferout;
		int ininvert, outinvert;

		int use_code;
		RString effect_exp[ 2 ];

		int inited;
		int codehandle[ 4 ];
		int need_recompile;
		CRITICAL_SECTION rcs;

		int AVS_EEL_CONTEXTNAME;
		double *var_beat, *var_alphain, *var_alphaout, *var_enabled, *var_clear, *var_w, *var_h;
		int isstart;

		int mode;

		int beat_render, beat_render_frames;
		int fake_enabled;
	};

	HRESULT stateDeclarations( EffectStateBuilder &builder ) { return E_NOTIMPL; }
};