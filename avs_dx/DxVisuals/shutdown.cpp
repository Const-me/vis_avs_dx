#include "stdafx.h"
#include "Utils/effectsRegistry.h"
#include "Resources/staticResources.h"
#include "Effects/Video/MF/mfStatic.h"
#include "../InteropLib/deviceCreation.h"

// Called on the rendering thread, immediately before shutting down. The GUI thread is waiting for this function to succeed.
HRESULT onRenderThreadShuttingDown()
{
	destroyEffects();
	StaticResources::destroy();
	mfShutdown();
	destroyDevice();
	comUninitialize();
	return S_OK;
}