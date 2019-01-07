#include "stdafx.h"
#include "effects.h"
#include "Resources/staticResources.h"
#include "Effects/Video/MF/mfStatic.h"
#include "Render/Profiler.h"
#include "../InteropLib/deviceCreation.h"
#include "Transition.h"

// Called on the rendering thread, immediately before shutting down. The GUI thread is waiting for this function to succeed.
void onRenderThreadShuttingDown( bool renderThread )
{
	destroyAllEffects();
	destroyTransitionInstance();
	StaticResources::destroy();
	if( renderThread )
		mfShutdown();
	gpuProfiler().shutdown();
	destroyDevice();
	if( renderThread )
		comUninitialize();
}