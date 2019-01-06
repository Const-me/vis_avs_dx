#pragma once
#include "../../EffectImpl.hpp"
#include "../../../Expressions/CompiledShader.h"

using namespace Hlsl::Trans::ColorModifier;

struct ColorModifierStructs
{
	struct AvsState
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

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Expressions::CompiledShader<ColorModifierPS>;

	struct StateData : public Expressions::Compiler
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs )
		{
			return Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
		}
	};
};

class ColorModifier : public EffectBase1<ColorModifierStructs>
{
public:
	inline ColorModifier( AvsState *pState ) : tBase( pState ) { }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};