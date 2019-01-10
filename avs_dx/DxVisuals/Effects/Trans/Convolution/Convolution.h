#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Effects/Common/ApeEffectBase.h>

struct ConvolutionStructs
{
	using AvsState = C_RBASE;
	using StateData = EmptyStateData;
	struct CsData : public ShaderUpdatesSimple<CsData, Hlsl::Trans::Convolution::ConvolutionCS>
	{
		CsData() = default;
		CsData( const C_RBASE& nativeFx );

		void dispatch() const;
	};
};

class Convolution : public EffectBase1<ConvolutionStructs, ApeEffectBase>
{
public:
	Convolution( C_RBASE* pThis );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};