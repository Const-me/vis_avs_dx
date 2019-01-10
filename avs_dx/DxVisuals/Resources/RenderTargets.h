#pragma once
#include "RenderTarget.h"
#include "../Utils/resizeHandler.h"

// A set of 2 render target textures.
class RenderTargets
{
	array<RenderTarget, 2> m_targets;
	uint8_t m_lastTarget = 0;

public:
	RenderTargets() = default;
	RenderTargets( const RenderTargets & ) = delete;
	void operator=( const RenderTargets & ) = delete;

	void destroy()
	{
		for( auto& t : m_targets )
			t.destroy();
	}

	// Bind last written RT for output, optionally clear. This does not advance to the next target.
	HRESULT writeToLast( bool clear = false );

	// Bind last written RT for input, advance to the next target, bind next one for output. Optionally clear the output.
	HRESULT writeToNext( BoundPsResource& bound, bool clearNext = false );

	// Copy last written RT to the next one, advance to the next target, bind last written RT for input, next one for output.
	HRESULT blendToNext( BoundPsResource& bound );

	RenderTarget& lastWritten()
	{
		return m_targets[ m_lastTarget ];
	}

	const uint8_t currentIndex() const
	{
		return m_lastTarget;
	}

	HRESULT computeToNext( UINT readSlot, BoundSrv<eStage::Compute>& boundRead, UINT writeSlot, BoundUav& boundWrite );
};

// Same as above but also automatically destroys itself when rendering window is resized.
class RenderTargetsAutoDrop: public RenderTargets,
	public ResizeHandler
{
	void onRenderSizeChanged() override
	{
		destroy();
	}
};