#include "stdafx.h"
#include "GlobalBuffers.h"

void GlobalBuffers::destroy()
{
	for( RenderTargets& rt : m_buffers )
		rt.destroy();
}