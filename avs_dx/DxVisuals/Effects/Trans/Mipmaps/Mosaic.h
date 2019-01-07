#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include "MipMaps.h"

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

	using PsData = Hlsl::Trans::Mipmaps::MosaicPS;
};

class Mosaic : public EffectBase1<MosaicStructs>,
	public MipMapsRenderer
{
	CComPtr<ID3D11Buffer> m_cb;
	HRESULT renderDisabled( RenderTargets& rt );

public:

	Mosaic( AvsState* avs );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};