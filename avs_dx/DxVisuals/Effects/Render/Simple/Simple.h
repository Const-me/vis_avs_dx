#pragma once
#include "../../EffectImpl.hpp"
#include "../PointSpritesRender.hpp"
#include <variant>
using namespace Hlsl::Render::Simple;

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

class SimpleDotsFx : public EffectBase1<DotsRendering>
{
public:
	SimpleDotsFx( AvsState *pState ) : tBase( pState ) { }

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

class SimpleSolidFx : public EffectBase1<SolidRendering>
{
public:
	SimpleSolidFx( AvsState *pState ) : tBase( pState ) { }

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

class SimpleLinesFx : public EffectBase1<LinesRendering>
{
public:
	SimpleLinesFx( AvsState *pState ) : tBase( pState ) { }

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

// ==== The effect itself ====
class Simple : public EffectBase1<SimpleBase>
{
	eSimpleStyle m_style;
	std::variant<std::monostate, SimpleSolidFx, SimpleDotsFx, SimpleLinesFx> m_impl;
	EffectBase* m_pImpl = nullptr;

	bool replaceStyleIfNeeded();

public:
	Simple( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT shouldRebuildState() override;

	void setStateOffset( UINT off ) override;

	// Forward the rest of the calls to specific renderers
	HRESULT updateParameters( Binder& binder ) override { return m_pImpl->updateParameters( binder ); }

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};