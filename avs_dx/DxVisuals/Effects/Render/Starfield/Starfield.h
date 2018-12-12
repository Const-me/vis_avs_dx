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

	struct StateData: public StarfieldStateStruct
	{
		UINT stateSize() { return 3; }

		StateData( const AvsState& s )
		{
			onbeat = s.onbeat ? true : false;
			spdBeat = s.spdBeat;
			WarpSpeed = s.WarpSpeed;
			durFrames = s.durFrames;
		}
	};
};

class Starfield : public EffectBase1<StarfieldStructs>
{
public:

	inline Starfield( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override { return E_NOTIMPL; }
};