#pragma once
#include "../../EffectImpl.hpp"
#include "MosaicTexture.h"

struct MosaicStructs
{
	struct MosaicCb
	{
		float2 cellsCount;
		float LOD;
		uint32_t zzUnused;
	};

	struct AvsState
	{
		int enabled;
		int quality;
		int quality2;
		int blend;
		int blendavg;
		int onbeat;
		int durFrames;
		int nF;
		int thisQuality;

		MosaicCb update( bool isBeat );
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Trans::Mosaic::MosaicPS;
};

class Mosaic : public EffectBase1<MosaicStructs>
{
	MosaicTexture m_texture;
	CComPtr<ID3D11Buffer> m_cb;
#ifdef DEBUG
	CComPtr<ID3D11SamplerState> m_sampler;	// To make debug layer shut up about missing sampler
#endif

	HRESULT renderDisabled( RenderTargets& rt );

public:
	Mosaic( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};