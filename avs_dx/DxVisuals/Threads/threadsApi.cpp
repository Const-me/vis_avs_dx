#include "stdafx.h"
#include "threadsApi.h"
#include "AvsThreads.h"

HRESULT avsThreadsStart( winampVisModule *this_mod )
{
	return getThreads().start( this_mod );
}

HRESULT avsThreadsStop()
{
	return getThreads().stop();
}

bool avsThreadsShuttingDown()
{
	return getThreads().shuttingDown();
}

GuiThread& getGuiThread()
{
	return getThreads().gui();
}

CComAutoCriticalSection renderLock;