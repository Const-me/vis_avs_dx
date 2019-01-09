#include "stdafx.h"
#include "windowProc.h"
#include "RenderWindow.h"
#include "interop.h"

void dxWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LRESULT res;
	RenderWindow::instance().ProcessWindowMessage( hwnd, message, wParam, lParam, res );
}

HRESULT presentSingle( const RenderTarget& src )
{
	return RenderWindow::instance().presentSingle( src );
}

HRESULT presentTransition( const RenderTarget& t1, const RenderTarget& t2, int trans, float sintrans )
{
	return RenderWindow::instance().presentTransition( t1, t2, trans, sintrans );
}