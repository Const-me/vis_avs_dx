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
	logShutdown( "~RenderThreadImpl #1" );
	// This will call the GUI thread to destroy rendering and profiler windows, so we can cleanup D3D device and associated resources.
	// Some resources, notably the video engine, have thread affinity, they must be released before MfShutdown() and CoUninitialize() calls.
	// Besides, we want to be sure the GUI thread will no longer present these frames, doing that after resources are released would prolly crash Winamp.
	RenderWindow::instance().shutdown();
	logShutdown( "~RenderThreadImpl, #2" );

	destroyAllEffects();
	destroyTransitionInstance();
	StaticResources::destroy();
	mfShutdown();
	gpuProfiler().shutdown();
	destroyDevice();
	comUninitialize();

	logShutdown( "~RenderThreadImpl #3" );
}

HRESULT RenderThreadImpl::initialize()
{
	return RenderThread::startup();
}

CSize getRenderSize();

HRESULT RenderThreadImpl::run()
{
	while( !shouldQuit() )
	{
		RenderThread::renderFrame();
		const float fps = m_fps.update();
		RenderThread::updateStats( getRenderSize(), fps );
	}
	return S_OK;
}