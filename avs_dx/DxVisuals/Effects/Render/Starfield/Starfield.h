#pragma once
#include "../../EffectImpl.hpp"
#include "../PointSpritesRender.hpp"
using namespace Hlsl::Render::Starfield;

struct StarfieldStructs: public PointSpritesRender
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
		int MaxStars_set;
		float WarpSpeed;
		int blend;
		int blendavg;
		int onbeat;
		float spdBeat;
		float incBeat;
		int durFrames;
		float CurrentSpeed;
	};

	struct StateData: public StarfieldState
	{
		UINT stateSize() { return 3; }

		StateData( const AvsState& s );
	};

	using CsData = StarCS;

	class VsData : public StarVS
	{
		int m_color = -1;
	public:
		HRESULT updateAvs( const AvsState& ass );
	};

	static const CAtlMap<CStringA, CStringA>& effectIncludes() { return includes(); }
};

class Starfield : public EffectBase1<StarfieldStructs>
{
	StructureBuffer starsBuffer;
	int prevStars = 0;
	UINT nStars, nComputeGroups;

public:

	Starfield( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT initializedState() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};