#include "stdafx.h"
#include "EffectList.h"

struct EffectList::T_RenderListType
{
	C_RBASE *render;
	int effect_index;
	int has_rbase2;
};

struct EffectList::AvsState
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

IMPLEMENT_EFFECT( EffectList, C_RenderListClass )