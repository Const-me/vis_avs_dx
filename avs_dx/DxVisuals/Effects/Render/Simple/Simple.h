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

	struct DotsRendering : public PointSpritesRender
	{
		using CsData = SimpleCS;
		using VsData = SimpleDotsVS;
	};

	struct SolidRendering
	{
		struct VsData
		{
			// Offset in state buffer to read the current color, in 32-bit elements.
			int effectState;
			// start/end Y coordinate, in clip space units
			float y1, y2;
			HRESULT defines( Hlsl::Defines& def ) const;
		};
		struct PsData
		{
			float readV;
			bool analizer;
			HRESULT defines( Hlsl::Defines& def ) const;
		};
	};
};

/* class Simple: public SimpleBase, public EffectBase
{
	SimpleBase::AvsState* const avs;
	StateData stateData;

public:
	using SimpleBase::AvsState;

	Simple( AvsState* p ) : avs( p ), stateData( *p ){ }

	const Metadata& metadata() override;

	HRESULT shouldRebuildState() override
	{
		return stateData.update( *avs );
	}

	HRESULT buildState( EffectStateShader& ess ) override
	{
		ess.shaderTemplate = stateData.shaderTemplate();
		ess.stateSize = stateData.stateSize();
		return stateData.defines( ess.values );
	}

	HRESULT render( RenderTargets& rt ) override { return E_NOTIMPL; }
}; */

// For the first version, hardcode the "Dots" method
class Simple : public EffectBase1<SimpleBase>
{
	StructureBuffer dotsBuffer;

public:
	Simple( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};