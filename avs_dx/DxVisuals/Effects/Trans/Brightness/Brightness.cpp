#include "stdafx.h"
#include <Effects/Common/EffectImpl.hpp>

struct BrightnessStruct
{
	struct AvsState
	{
		int enabled;
		int redp, greenp, bluep;
		int blend, blendavg;
		int dissoc;
		int color;
		int exclude;
		int distance;
	};

	static ID3D11VertexShader *vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}
	struct PsData : public ShaderUpdatesSimple<PsData, Hlsl::Trans::Brightness::BrightnessPS>
	{
		static inline float mulFromAvs( int a )
		{
			if( a <= 0 )
				return (float)( 4096 + a ) * ( 1.0f / 4096.0f );
			return (float)a * ( 16.0f / 4096.0f ) + 1.0f;
		}

		PsData() = default;
		PsData( const AvsState& s )
		{
			values.x = mulFromAvs( s.redp );
			values.y = mulFromAvs( s.greenp );
			values.z = mulFromAvs( s.bluep );
			exclude = !!s.exclude;
			excludeColor = float3FromColor( s.color );
			excludeDistance = (float)s.distance * ( 1.0f / 255.0f );
		}
	};
	using StateData = EmptyStateData;
};

class Brightness : public EffectBase1<BrightnessStruct>
{
public:
	Brightness( AvsState* p ) : tBase( p ) { }

private:
	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override
	{
		if( !avs->enabled )
			return S_FALSE;

		if( !renderer.bindShaders( false ) )
			return S_FALSE;

		BoundPsResource bound;
		if( avs->blend )
		{
			CHECK( rt.blendToNext( bound ) );
			omBlend( eBlend::Add );
		}
		else if( avs->blendavg )
		{
			CHECK( rt.blendToNext( bound ) );
			omCustomBlend( 0.5f );
		}
		else
		{
			CHECK( rt.writeToNext( bound ) );
			omBlend( eBlend::None );
		}
		drawFullscreenTriangle( false );
		return S_OK;
	}
};

IMPLEMENT_EFFECT( Brightness, C_BrightnessClass )