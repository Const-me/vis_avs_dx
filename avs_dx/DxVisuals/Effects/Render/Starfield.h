#pragma once
#include "../EffectImpl.hpp"

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
};

class Starfield : public EffectBase1<StarfieldStructs>
{
public:

	inline Starfield( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override
	{
		return E_NOTIMPL;
	}

	HRESULT render( RenderTargets& rt ) override { return E_NOTIMPL; }
};