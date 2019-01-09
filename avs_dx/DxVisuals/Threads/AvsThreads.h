#pragma once
#include "GuiThreadImpl.h"
#include "RenderThreadImpl.h"

// The main class implementing 2 main threads used by AVS, GUI threads who owns all windows and runs a message pump, and rendering thread.
class AvsThreads
{
	winampVisModule *this_mod = nullptr;

	struct ThreadInfo
	{
		CHandle hThread;

		HRESULT launch( LPTHREAD_START_ROUTINE fnMain, ThreadBase **ppDest, HANDLE hLaunchedEvent );

		operator HANDLE () const { return hThread; }
	};

	template<class T>
	static DWORD __stdcall threadMain( void* ppvDest );

	HRESULT threadMain2( ThreadBase& tb, ThreadBase **ppDest );

	ThreadInfo m_gui, m_render;
	CHandle evtLaunched;

	GuiThreadImpl* m_pGui;
	RenderThreadImpl* m_pRender;

	HRESULT launchBothThreads();
	volatile bool m_bShuttingDown = false;

public:

	HRESULT start( winampVisModule *this_mod );
	HRESULT stop();

	GuiThreadImpl& gui() { return *m_pGui; }

	RenderThreadImpl& render() { return *m_pRender; }

	bool shuttingDown() { return m_bShuttingDown; }
};

AvsThreads& getThreads();

#define g_hwnd getThreads().gui().visual()