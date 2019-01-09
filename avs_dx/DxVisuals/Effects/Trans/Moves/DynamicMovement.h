#pragma once
#include "MovementCommon.h"

struct DynamicMovementStructs: public MovementStructs
{
	struct AvsState
	{
		RString effect_exp[ 4 ];
		int m_xres, m_yres;
		int buffern;
		int subpixel, rectcoords, blend, wrap, nomove;
	};

	struct StateData : public CommonStateData
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs )
		{
			BoolHr hr = Compiler::update( avs.effect_exp[ 3 ].get(), avs.effect_exp[ 1 ].get(), avs.effect_exp[ 2 ].get(), avs.effect_exp[ 0 ].get() );
			hr.combine( updateScreenSize() );
			return hr;
		}

		HRESULT defines( Hlsl::Defines& def ) const;
	};

	struct VsData : public CommonVsData
	{
		HRESULT updateAvs( const AvsState& avs )
		{
			return CommonVsData::updateAvs( avs.rectcoords );
		}

		HRESULT defines( Hlsl::Defines& def ) const;
	};

	using PsData = DMovePS;
};

class DynamicMovement : public EffectBase1<DynamicMovementStructs>, public MovementFx
{
public:
	DynamicMovement( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};