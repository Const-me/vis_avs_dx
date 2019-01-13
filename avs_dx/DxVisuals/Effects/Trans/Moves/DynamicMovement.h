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

		HRESULT update( AvsState& avs );
		HRESULT defines( Hlsl::Defines& def ) const;
	};

	struct VsData : public CommonVsData
	{
		HRESULT updateAvs( const AvsState& avs );
		HRESULT defines( Hlsl::Defines& def ) const;
	};

	struct PsData : public DMovePS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
};

class DynamicMovement : public EffectBase1<DynamicMovementStructs>, public MovementFx
{
public:
	DynamicMovement( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};