#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Expressions/CompiledShader.h>
#include "../Moves/Sampler.h"

struct DynamicShiftStructs
{
	struct AvsState
	{
		RString effect_exp[ 3 ];
		int blend, subpixel;
	};

	struct StateData : public Expressions::Compiler
	{
		StateData( AvsState& ass );

		HRESULT update( AvsState& avs );

		HRESULT defines( Hlsl::Defines& def ) const;

	private:
		CSize screenSize;
		bool blending = true;
	};

	using VsData = Hlsl::Trans::DynamicShift::DynamicShiftVS;

	struct PsData : public Hlsl::Trans::Moves::DMovePS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
};

class DynamicShift : public EffectBase1<DynamicShiftStructs>
{
	Sampler m_sampler;

public:
	DynamicShift( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};