#pragma once
#include "../../EffectImpl.hpp"
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

	struct PsData: public ColorModifierPS
	{
		PsData();

		const ShaderTemplate* shaderTemplate() { return &m_template; }

	private:
		CStringA m_hlsl;
		ShaderTemplate m_template;
	};

	struct StateData : public Expressions::Compiler
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& ass )
		{
			return hr_or( Compiler::update( ass.effect_exp ), updateInputs( ass ) );
		}

		HRESULT defines( Hlsl::Defines& def ) const;

	private:
		CSize screenSize;
		HRESULT updateInputs( const AvsState& ass );
	};
};

class ColorModifier : public EffectBase1<ColorModifierStructs>
{
public:
	inline ColorModifier( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( RenderTargets& rt ) override;
};