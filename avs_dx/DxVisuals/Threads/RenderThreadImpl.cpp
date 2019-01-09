#include "stdafx.h"
#include "RenderThreadImpl.h"
#include "AvsThreads.h"

#include "effects.h"
#include "Resources/staticResources.h"
#include "Effects/Video/MF/mfStatic.h"
#include "Render/Profiler.h"
#include <Interop/deviceCreation.h>
#include "Transition.h"

RenderThreadImpl::RenderThreadImpl( winampVisModule * pModule ) :
	RenderThread( pModule )
{ }

RenderThreadImpl::~RenderThreadImpl()
{
	// getThreads().render()

	destroyAllEffects();
	destroyTransitionInstance();
	StaticResources::destroy();
	mfShutdown();
	gpuProfiler().shutdown();
	destroyDevice();
	comUninitialize();
}

HRESULT RenderThreadImpl::initialize()
{
	return RenderThread::startup();
}

HRESULT RenderThreadImpl::run()
{
	while( 0 == m_ThreadQuit )
		RenderThread::renderFrame();
	return S_OK;
}