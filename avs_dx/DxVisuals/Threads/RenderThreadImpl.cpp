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

HRESULT RenderThreadImpl::run()
{
	while( !shouldQuit() )
		RenderThread::renderFrame();
	return S_OK;
}