#pragma once
#include "ThreadBase.h"

// Implemented in vis_avs.dll, linker is awesome.
// But physically, this class lives on the stack of the GUI thread, i.e. destructor will destroy all that stuff.
class GuiThread: public ThreadBase
{
	HWND m_hwnd = nullptr;
	HWND m_hwndDlg = nullptr;

public:
	GuiThread( winampVisModule * pModule ) :
		ThreadBase( pModule ) { }

	HWND visual() { return m_hwnd; }
	HWND config() { return m_hwndDlg; }

	void setVisualWindow( HWND w ) { m_hwnd = w; }
	void clearVisualWindow() { m_hwnd = nullptr; }

	void setConfigWindow( HWND w ) { m_hwndDlg = w; }
	void clearConfigWindow() { m_hwndDlg = nullptr; }

protected:

	HRESULT startup();

	void shutdown();

	void configOpen();

	void configClose();
};