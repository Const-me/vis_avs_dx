#pragma once
#include "RenderTarget.h"
#include "../../InteropLib/interop.h"

// A set of 2 render targets textures.
class RenderTargets
{
	std::array<RenderTarget, 2> m_targets;
	size_t m_lastTarget = 0;

public:
	void destroy()
	{
		for( auto& t : m_targets )
			t.destroy();
	}

	// Bind last written RT for output, optionally clear
	HRESULT writeToLast( bool clear = false );

	// Bind last written RT for input, next one for output, optionally clear the output.
	HRESULT writeToNext( UINT readPsSlot, bool clearNext );

	// Copy last written RT to the next one, bind last written RT for input, next one for output.
	HRESULT blendToNext( UINT readPsSlot );

	RenderTarget& lastWritten()
	{
		return m_targets[ m_lastTarget ];
	}

	void swapLast( RenderTarget& dest );
};