#pragma once
#include "MovementCommon.h"
#include <EASTL/variant.h>

struct StaticMovementStructs: public MovementStructs
{
	struct AvsState
	{
		RString effect_exp;
		int effect_exp_ch;
		int effect, blend;
		int sourcemapped;
		int rectangular;
		int subpixel;
		int wrap;
	};

	struct StateData : public CommonStateData
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs );

		HRESULT defines( Hlsl::Defines& def ) const;
	};

	struct VsData : public CommonVsData
	{
		HRESULT updateAvs( const AvsState& avs )
		{
			return CommonVsData::updateAvs( avs.rectangular );
		}

		HRESULT defines( Hlsl::Defines& def ) const;
	};
};

class Movement : public EffectBase1<StaticMovementStructs>, public MovementFx
{
	using DynamicShader = Shader<DMovePS>;
	using FuzzifyShader = Shader<FuzzifyPS>;
	using BlockyOutShader = Shader<BlockyOutPS>;

	eastl::variant<DynamicShader, FuzzifyShader, BlockyOutShader> m_ps;

	template<class S>
	HRESULT updateShader( S& ps, Binder& binder );

	template<class S>
	HRESULT renderFullscreen( S& ps, RenderTargets& rt );

public:
	Movement( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};