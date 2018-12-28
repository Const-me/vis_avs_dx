#pragma once
#include "../../EffectImpl.hpp"
#include <Utils/FadeOut.h>
#include <Effects/shadersCode.h>

struct BlitterStructs
{
	struct AvsState
	{
		int scale, scale2, blend, beatch;
		int fpos;
		int subpixel;
	};

	using StateData = EmptyStateData;

	static ByteRange vertexShaderBinary()
	{
		return Hlsl::StaticShaders::BlitterVS();
	}
	static ByteRange pixelShaderBinary()
	{
		return Hlsl::StaticShaders::BlitterPS();
	}
};

class Blitter : public EffectBase1<BlitterStructs>
{
	FadeOut m_fade;

public:
	Blitter( AvsState* avs ) : EffectBase1( avs ) { }

	DECLARE_EFFECT();

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};