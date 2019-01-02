#pragma once
#include "MovementCommon.h"

struct DynamicDistanceStructs : public MovementStructs
{
	struct AvsState
	{
		RString effect_exp[ 4 ];
		int blend;
		int subpixel;
	};

	struct StateData : public CommonStateData
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs )
		{
			return Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
		}
	};

	struct VsData : public Expressions::CompiledShader<DynamicDistanceVS>
	{
		static HRESULT compiledShader( const std::vector<uint8_t>& dxbc )
		{
			return StaticResources::createLayout( dxbc );
		}

		HRESULT updateAvs( const AvsState& avs )
		{
			const CSize rs = getRenderSize();
			Vector2 diag{ (float)rs.cx, (float)rs.cy };
			diag.Normalize();
			return updateValue( scaleToUniform, diag );
		}
	};

	using PsData = DMovePS;
};

class DynamicDistance : public EffectBase1<DynamicDistanceStructs>, public MovementFx
{
public:
	DynamicDistance( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};