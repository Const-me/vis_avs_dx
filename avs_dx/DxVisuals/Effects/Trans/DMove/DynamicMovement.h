#pragma once
#include <Effects/EffectImpl.hpp>
#include <Expressions/CompiledShader.h>
#include <Resources/GridMesh.h>
#include <Utils/resizeHandler.h>
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

		HRESULT update( AvsState& avs )
		{
			BoolHr hr = Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
			hr.combine( updateInputs( avs ) );
			return hr;
		}

		HRESULT defines( Hlsl::Defines& def ) const;

	private:
		CSize screenSize;
		HRESULT updateInputs( const AvsState& ass );
	};

	struct VsData : public Expressions::CompiledShader<DMoveVS>
	{
		static HRESULT compiledShader( const std::vector<uint8_t>& dxbc );

		HRESULT updateAvs( const AvsState &avs );
	};

	using PsData = DMovePS;
};

class DynamicMovement : public EffectBase1<DynamicMovementStructs>, public ResizeHandler
{
	GridMesh m_mesh;

	void onRenderSizeChanged() override;

public:
	DynamicMovement( AvsState *pState );

	const Metadata& metadata() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};