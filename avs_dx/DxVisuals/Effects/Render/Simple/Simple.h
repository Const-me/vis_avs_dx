#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include "../PointSpritesRender.hpp"
#include <EASTL/variant.h>
using namespace Hlsl::Render::Simple;

// Ironically, because of 3 very different rendering styles, this "Simple" is one of the most complex effects.

// ==== Common stuff for all styles ====
enum struct eSimpleStyle : uint8_t
{
	Dots,
	Solid,
	Lines
};

// The effect structure for the main effect. It does state management, but all 4 shaders are empty.
struct SimpleBase
{
public:
	struct AvsState
	{
		int effect;
		int num_colors;
		int colors[ 16 ];
		int color_pos;

		eSimpleStyle style() const;

		float sampleV() const;

		float2 positionY() const;
	};

	struct StateData : public SimpleState
	{
		StateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 4; }
	};
};

__interface iSimpleRenderer
{
	HRESULT updateParameters( Binder& binder, const SimpleBase::AvsState& avs, const SimpleBase::StateData& dx );
	HRESULT compileShaders( UINT stateOffset );
	HRESULT render( bool isBeat, RenderTargets& rt );
};

template<class FxDef>
class SimpleRenderer : public EffectRenderer<FxDef>, public iSimpleRenderer
{
public:
	using AvsState = SimpleBase::AvsState;
	SimpleRenderer() = default;
	HRESULT updateParameters( Binder& binder, const SimpleBase::AvsState& avs, const SimpleBase::StateData& dx ) override
	{
		return __super::update( binder, avs, dx );
	}
	HRESULT compileShaders( UINT stateOffset ) override
	{
		return EffectRenderer<FxDef>::compileShaders( stateOffset );
	}
};

// ==== Dots rendering ====
struct DotsRendering : public PointSpritesRender
{
	using AvsState = SimpleBase::AvsState;
	using StateData = EmptyStateData;

	struct VsData : public SimpleDotsVS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
};
class SimpleDotsFx : public SimpleRenderer<DotsRendering>
{
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

// ==== Solid rendering ====
struct SolidRendering
{
	using AvsState = SimpleBase::AvsState;
	using StateData = EmptyStateData;

	struct VsData : public SimpleSolidVS
	{
		HRESULT updateAvs( const AvsState& avs );
	};

	struct PsData : public SimpleSolidPS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
};
class SimpleSolidFx : public SimpleRenderer<SolidRendering>
{
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

// ==== Lines rendering ====
struct LinesRendering
{
	using AvsState = SimpleBase::AvsState;
	using StateData = EmptyStateData;

	struct VsData : public SimpleLinesVS
	{
		HRESULT updateAvs( const AvsState& avs );
	};

	using GsData = Hlsl::Render::PolylineGS;

	using PsData = Hlsl::Render::PolylinePS;
};
class SimpleLinesFx : public SimpleRenderer<LinesRendering>
{
	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

// ==== The effect itself ====
class Simple : public EffectBase1<SimpleBase>
{
	eSimpleStyle m_style;
	eastl::variant<eastl::monostate, SimpleSolidFx, SimpleDotsFx, SimpleLinesFx> m_impl;
	iSimpleRenderer* m_pImpl = nullptr;

	bool replaceStyleIfNeeded();

public:
	Simple( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT shouldRebuildState() override;

	// Forward the rest of the calls to specific renderers
	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};