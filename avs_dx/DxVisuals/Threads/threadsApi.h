#pragma once
#include "GuiThread.h"
#include "RenderThread.h"

HRESULT avsThreadsStart( winampVisModule *this_mod );

HRESULT avsThreadsStop();

GuiThread& getGuiThread();

bool avsThreadsShuttingDown();

extern ATL::CComAutoCriticalSection renderLock;

enum eUpdate: uint8_t
{
	// Single effect has changed
	Params = 1,
	// Effect was aded/removed/reordered
	Effects = 2,
	// Preset load or undo
	Preset = 0x10,
};

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