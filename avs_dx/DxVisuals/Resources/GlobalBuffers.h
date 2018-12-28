#pragma once
#include "RenderTargets.h"
#include "../Utils/resizeHandler.h"

class GlobalBuffers: public ResizeHandler
{
	std::array<RenderTargets, 8> m_buffers;

	void onRenderSizeChanged() override
	{
		destroy();
	}

public:

	void destroy();

	RenderTargets& operator[]( uint8_t i ) { return m_buffers[ i ]; }
};