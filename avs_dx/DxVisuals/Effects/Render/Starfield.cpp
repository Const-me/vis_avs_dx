#include "stdafx.h"
#include "Starfield.h"

struct Starfield::StarFormat
{
	int X, Y;
	float Z;
	float Speed;
	int OX, OY;
};

struct Starfield::AvsState
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

IMPLEMENT_EFFECT( Starfield, C_StarField )