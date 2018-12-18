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

HRESULT presentSingle( const RenderTarget& src )
{
	return wnd.presentSingle( src );
}

HRESULT presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans )
{
	return wnd.presentTransition( t1, t2, trans, sintrans );
}