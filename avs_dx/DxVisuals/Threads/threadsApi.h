#pragma once
#include "GuiThread.h"
#include "RenderThread.h"

HRESULT avsThreadsStart( winampVisModule *this_mod );

HRESULT avsThreadsStop();

GuiThread& getGuiThread();