#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )

struct ColorModifier::AvsState
{
	RString effect_exp[ 4 ];

	int m_recompute;

	int m_tab_valid;
	unsigned char m_tab[ 768 ];
	int AVS_EEL_CONTEXTNAME;
	double *var_r, *var_g, *var_b, *var_beat;
	int inited;
	int codehandle[ 4 ];
	int need_recompile;
	CRITICAL_SECTION rcs;
};