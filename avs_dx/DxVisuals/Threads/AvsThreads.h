#pragma once
#include "GuiThreadImpl.h"
#include "RenderThreadImpl.h"

class AvsThreads
{
	winampVisModule *this_mod = nullptr;
	CComAutoCriticalSection m_csRendering;

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

public:

	HRESULT start( winampVisModule *this_mod );
	HRESULT stop();

	CComAutoCriticalSection& csRender() { return m_csRendering; }

	GuiThreadImpl& gui() { return *m_pGui; }

	RenderThreadImpl& render() { return *m_pRender; }
};

AvsThreads& getThreads();

#define RENDER_LOCK() CSLock __lock{ getThreads().csRender() }

#define g_hwnd getThreads().gui().visual()