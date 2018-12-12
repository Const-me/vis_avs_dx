#pragma once
#include "../../EffectImpl.hpp"
using namespace Hlsl::Render::Starfield;

struct StarfieldStructs
{
	struct StarFormat
	{
		int X, Y;
		float Z;
		float Speed;
		int OX, OY;
	};

	struct AvsState
	{
		int enabled;
		int color;
		int MaxStars, MaxStars_set;
		int Xoff;
		int Yoff;
		int Zoff;
		float WarpSpeed;
		int blend;
		int blendavg;
		StarFormat Stars[ 4096 ];
		int Width, Height;
		int onbeat;
		float spdBeat;
		float incBeat;
		int durFrames;
		float CurrentSpeed;
		int nc;
	};

	struct StateData
	{
		const StateShaderTemplate& shaderTemplate() { return StarfieldState(); }

		UINT stateSize() { return 3; }

		StateData()
		{

		}
		StateData( const AvsState& s )
		{
			onbeat = s.onbeat ? true : false;
			spdBeat = s.spdBeat;
			WarpSpeed = s.WarpSpeed;
			durFrames = s.durFrames;
		}

		HRESULT defines( StateMacroValues& vals ) const
		{
			vals.add( "onbeat", onbeat );
			vals.add( "spdBeat", spdBeat );
			vals.add( "WarpSpeed", WarpSpeed );
			vals.add( "durFrames", durFrames );
			return S_OK;
		}

		bool onbeat;
		float spdBeat;
		float WarpSpeed;
		uint32_t durFrames;
	};
};

class Starfield : public EffectBase1<StarfieldStructs>
{
public:

	inline Starfield( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override { return E_NOTIMPL; }
};