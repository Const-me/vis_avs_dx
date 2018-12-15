#pragma once
#include "../../EffectImpl.hpp"
#include "../PointSpritesRender.hpp"
using namespace Hlsl::Render::Simple;

class SimpleBase
{
public:
	struct AvsState
	{
		int effect;
		int num_colors;
		int colors[ 16 ];
		int color_pos;

		float sampleV() const;
	};

	struct StateData: public SimpleState
	{
		StateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 4; }
	};
};

struct DotsRendering : public SimpleBase, public PointSpritesRender
{
	class CsData : public SimpleCS
	{
		int m_effect = -1;

	public:

		HRESULT updateAvs( const AvsState& avs );
	};

	using VsData = SimpleDotsVS;
};

// For the first version, hardcode the "Dots" method
class Simple : public EffectBase1<DotsRendering>
{
	StructureBuffer dotsBuffer;

public:
	Simple( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT initializedState() override;

	HRESULT render( RenderTargets& rt ) override;
};