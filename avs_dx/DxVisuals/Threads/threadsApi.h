#pragma once
// We don't want to #include AvsThreads.h into vis_avs project, it depends on too much stuff like D3D and WTL.
// That's why exposing much simpler API for that project.

#include "GuiThread.h"
#include "RenderThread.h"

HRESULT avsThreadsStart( winampVisModule *this_mod );

HRESULT avsThreadsStop();

GuiThread& getGuiThread();

// Will start returning true after avsThreadsStop() is called.
bool avsThreadsShuttingDown();

extern ATL::CComAutoCriticalSection renderLock;

// Currently, the code ignores these update flags, but they can be marshaled to rendering thread and used to improve performance of effect updates.
enum eUpdate: uint8_t
{
	// Single effect has changed
	Params = 1,
	// Effects were added/removed/reordered
	Effects = 2,
	// Preset load or undo, i.e. everything has changed.
	Preset = 0x10,
};

// RAII class that locks renderLock CS while alive.
class Update
{
	const eUpdate update;
	ATL::CComCritSecLock<ATL::CComAutoCriticalSection> lock;

public:
	Update() = delete;
	Update( eUpdate u ) :
		update( u ), lock( renderLock ) { }

	~Update() = default;
};

#define UPDATE_PARAMS()  Update __upd{ eUpdate::Params }
#define UPDATE_EFFECTS() Update __upd{ eUpdate::Effects }
#define UPDATE_PRESET()  Update __upd{ eUpdate::Preset }