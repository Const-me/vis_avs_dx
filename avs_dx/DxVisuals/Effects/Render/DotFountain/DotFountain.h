#pragma once
#include "../../EffectImpl.hpp"
#include "../ParticleSystem.hpp"
using namespace Hlsl::Render::DotFountain;

struct DotFountainStructs : public PointSpritesRender
{
	struct AvsState
	{
		int rotvel, angle;
		int colors[ 5 ];
	};

	using StateData = EmptyStateData;

	struct CsData : public ShaderUpdatesSimple< CsData, FountainCS>
	{
		CsData() = default;
		CsData( const AvsState& a )
		{
			for( int i = 0; i < 5; i++ )
				colors[ i ] = float3FromColor( a.colors[ i ] );
		}
	};

	using InitCsData = FountainInitCS;

	using VsData = FountainVS;

	static const CAtlMap<CStringA, CStringA>& effectIncludes() { return includes(); }

	static constexpr UINT computeThreadsPerGroup = 32;
};

using DirectX::SimpleMath::Matrix;
using DirectX::XMConvertToRadians;

class DotFountain : public FacticleFx<DotFountainStructs>,
	public ResizeHandler
{
	// View * projection matrix
	Matrix m_viewProj;
	CComPtr<ID3D11Buffer> m_cb;
	float r = 0;

	void initMatrix();

	void onRenderSizeChanged() override;

public:

	DotFountain( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};