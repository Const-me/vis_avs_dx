#pragma once
#include "../../EffectImpl.hpp"

using namespace Hlsl::Render::MovingParticle;

struct MovingParticleStructs
{
	struct sVertex
	{
		Vector2 position;
		Vector3 color;
		float size;
	};

	struct AvsState
	{
		size_t vtbl;
		int enabled;
		int colors;
		int maxdist, size, size2;
		int blend;
		int s_pos;
		double c[ 2 ];
		double v[ 2 ];
		double p[ 2 ];

		sVertex update( int& s_pos, bool isBeat );
	};

	using StateData = EmptyStateData;

	struct VsData : public MovingParticleVS
	{
		static HRESULT compiledShader( const std::vector<uint8_t>& dxbc );
	};

	using GsData = MovingParticleGS;

	static inline ByteRange pixelShaderBinary()
	{
		return Hlsl::StaticShaders::MovingParticlePS();
	}
};

class MovingParticle : public EffectBase1<MovingParticleStructs>
{
	CComPtr<ID3D11Buffer> m_vb;
	int s_pos;

public:
	MovingParticle( AvsState *pState ) : tBase( pState ) { }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};