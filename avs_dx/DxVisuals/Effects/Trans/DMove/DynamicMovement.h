#pragma once
#include "../../EffectImpl.hpp"
using namespace Hlsl::Trans::DMove;

struct DynamicMovementStructs
{
	struct AvsState
	{
		RString effect_exp[ 4 ];

		int m_lastw, m_lasth;
		int m_lastxres, m_lastyres, m_xres, m_yres;
		int *m_wmul;
		int *m_tab;
		int AVS_EEL_CONTEXTNAME;
		double *var_d, *var_b, *var_r, *var_x, *var_y, *var_w, *var_h, *var_alpha;
		int inited;
		int codehandle[ 4 ];
		int need_recompile;
		int buffern;
		int subpixel, rectcoords, blend, wrap, nomove;
		CRITICAL_SECTION rcs;

		// smp stuff
		int __subpixel, __rectcoords, __blend, __wrap, __nomove;
		int w_adj;
		int h_adj;
		int XRES;
		int YRES;
	};

	struct StateData : public Expressions::Compiler
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& ass )
		{
			return Compiler::update( ass.effect_exp );
		}
	};

	using VsData = DMoveVS;

	using PsData = DMovePS;
};

class DynamicMovement : public EffectBase1<DynamicMovementStructs>
{
public:
	DynamicMovement( AvsState *pState );

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};