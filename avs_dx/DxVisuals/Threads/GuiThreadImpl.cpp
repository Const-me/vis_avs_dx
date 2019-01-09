#include "stdafx.h"
#include "GuiThreadImpl.h"
#include "Interop/profilerApi.h"

void GuiThreadImpl::postMessage( UINT Msg, WPARAM wParam, LPARAM lParam ) const
{
	const BOOL res = PostThreadMessage( threadId, Msg, wParam, lParam );
	assert( res );
}

HRESULT GuiThreadImpl::initialize()
{
	return GuiThread::startup();
}

HRESULT GuiThreadImpl::run()
{
	while( true )
	{
		MSG msg;
		if( !GetMessage( &msg, NULL, 0, 0 ) )
			break;	// WM_QUIT

		if( nullptr == msg.hwnd )
			if( handlePostedMessage( msg ) )
				continue;

		HWND wndConfig = GuiThread::config();
		if( nullptr != wndConfig && IsDialogMessage( wndConfig, &msg ) )
			continue;

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	GuiThread::shutdown();
	return S_OK;
}

GuiThreadImpl::~GuiThreadImpl()
{

}

bool GuiThreadImpl::handlePostedMessage( const MSG &msg )
{
	switch( msg.message )
	{
	case WM_CONFIG_OPEN:
		GuiThread::configOpen();
		return true;
	case WM_CONFIG_CLOSE:
		GuiThread::configClose();
		return true;
	}
	return false;
}

HRESULT GuiThreadImpl::closeWindows()
{
	if( !m_wndRender.isWindow() )
		return S_FALSE;
	return m_wndRender.shutdown();
}