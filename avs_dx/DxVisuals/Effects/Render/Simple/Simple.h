#pragma once
#include "../../EffectImpl.hpp"
#include "../PointSpritesRender.hpp"
#include <variant>
using namespace Hlsl::Render::Simple;

// ==== Common stuff for all styles ====
enum eSimpleRenderStyle : uint8_t
{
	Dots,
	Solid,
	Lines
};

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

	struct StateData : public SimpleState
	{
		StateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 4; }
	};

	struct ChildStateData : public EmptyStateData
	{
		eSimpleRenderStyle renderStyle;

		ChildStateData( const AvsState& avs );

		bool operator==( const ChildStateData& that )
		{
			return renderStyle == that.renderStyle;
		}
	};
};

// ==== Dots rendering ====

struct DotsRendering : public PointSpritesRender
{
	using AvsState = SimpleBase::AvsState;
	using StateData = SimpleBase::ChildStateData;

	struct CsData : public SimpleCS
	{
		HRESULT updateAvs( const AvsState& avs );
	};

	using VsData = SimpleDotsVS;
};

class SimpleDots : public EffectBase1<DotsRendering>
{
	StructureBuffer dotsBuffer;

public:
	SimpleDots( AvsState *pState ) : tBase( pState ) { }
	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};

// ==== Solid rendering ====

struct SolidRendering
{
	using AvsState = SimpleBase::AvsState;
	using StateData = SimpleBase::ChildStateData;
	struct VsData : public SimpleSolidVS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
	struct PsData : public SimpleSolidPS
	{
		HRESULT updateAvs( const AvsState& avs );
	};
};

class SimpleSolid : public EffectBase1<SolidRendering>
{
public:
	SimpleSolid( AvsState *pState ) : tBase( pState ) { }
	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};


// ==== The effect itself ====
class Simple : public EffectBase1<SimpleBase>
{
	eSimpleRenderStyle m_style;
	std::variant<std::monostate, SimpleSolid, SimpleDots> m_impl;
	EffectBase* m_pImpl = nullptr;

	bool replaceStyleIfNeeded();

public:
	Simple( AvsState *pState );

	const Metadata& metadata() override;

	HRESULT shouldRebuildState() override;

	// Forward the rest of the calls to specific renderers
	HRESULT updateParameters( Binder& binder ) override { return m_pImpl->updateParameters( binder ); }
	HRESULT render( RenderTargets& rt ) override { return m_pImpl->render( rt ); }
};