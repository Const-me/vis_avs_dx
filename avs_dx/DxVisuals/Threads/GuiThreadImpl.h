#pragma once
#include "GuiThread.h"
#include <Interop/RenderWindow.h>
#include <Interop/Profiler/ProfilerWindow.h>
#include "threadsApi.h"

// GUI thread owns all AVS windows.
class GuiThreadImpl : public GuiThread
{
protected:
	friend class AvsThreads;

	static inline const char* dbgGetThreadName() { return "AVS GUI thread"; }

	GuiThreadImpl( winampVisModule * pModule ) :
		GuiThread( pModule ) { }

	~GuiThreadImpl();

private:

	HRESULT initialize() override;

	HRESULT run() override;

	void postMessage( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 ) const;

	void postQuitMessage() override
	{
		postMessage( WM_QUIT );
	}

	bool handlePostedMessage( const MSG &msg );

	RenderWindow m_wndRender;

	static constexpr UINT WM_CONFIG_OPEN = WM_APP + 11;
	static constexpr UINT WM_CONFIG_CLOSE = WM_APP + 12;

public:

	// Render thread calls this during shutdown. It wants to shut down D3D context and COM, to do this cleanly the windows must be destroyed first, at least rendering and profiler. Only the owned thread can destroy windows.
	HRESULT closeWindows();
};