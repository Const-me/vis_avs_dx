#pragma once
#include <Effects/Common/EffectImpl.hpp>

struct InterferenceCb
{
	Vector4 alpha;
	Vector4 offsets[ 8 ];
};

struct InterferenceStructs
{
	struct AvsState
	{
		int enabled;
		int nPoints;
		int distance;
		int alpha;
		int rotation;
		int rotationinc;
		int distance2;
		int alpha2;
		int rotationinc2;
		int rgb;
		int blend;
		int blendavg;
		float speed;
		int onbeat;
		float a;
		int _distance;
		int _alpha;
		int _rotationinc;
		int _rgb;
		float status;

		InterferenceCb render( bool isBeat );
	};

	using StateData = EmptyStateData;

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::Interference::InterferencePS>
	{
		PsData() = default;
		PsData( const AvsState &s )
		{
			count = s.nPoints;
			separateRgb = s.rgb && ( 0 == ( s.nPoints % 3 ) );
		}
	};
};

class Interference : public EffectBase1<InterferenceStructs>
{
	CComPtr<ID3D11Buffer> m_cb;

public:
	Interference( AvsState* avs ) : EffectBase1( avs )
	{ }

	DECLARE_EFFECT()

	void bindResources() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};