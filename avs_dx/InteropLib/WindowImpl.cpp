#include "stdafx.h"
#include "windowProc.h"
#include "RenderWindow.h"
#include "interop.h"

static RenderWindow wnd;

void dxWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT res;
	wnd.ProcessWindowMessage( hwnd, message, wParam, lParam, res );
}

HRESULT present( const RenderTarget& src )
{
	return wnd.present( src );
}