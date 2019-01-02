#pragma once
#include "../../EffectImpl.hpp"
#include <Resources/dynamicCBuffer.h>

struct ColorFadeStructs
{
	struct AvsState
	{
		int enabled;
		int faders[ 3 ];
		int beatfaders[ 3 ];
		int faderpos[ 3 ];

		void update( bool isBeat );

		std::array<Vector4, 4> shaderConstants() const;
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Trans::ColorFade::ColorFadePS;
};

class ColorFade : public EffectBase1<ColorFadeStructs>
{
public:
	ColorFade( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

private:

	CComPtr<ID3D11Buffer> m_cb;
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};