#pragma once
#include "../EffectBase.h"

class Starfield : public EffectBase
{
public:
	Starfield();
	~Starfield();

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override;

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