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

GuiThread& getGuiThread()
{
	return getThreads().gui();
}