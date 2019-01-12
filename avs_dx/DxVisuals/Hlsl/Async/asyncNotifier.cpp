#include "stdafx.h"
#include "asyncNotifier.h"
#include <Interop/waitCursor.h>

static volatile long s_runningJobs = 0;
extern HWND g_hwndDlg;

void notifyJobStarted()
{
	if( 1 == InterlockedIncrement( &s_runningJobs ) )
	{
		if( nullptr != g_hwndDlg && IsWindow( g_hwndDlg ) )
			PostMessage( g_hwndDlg, WM_WAIT_CURSOR, TRUE, 0 );
	}
}

void notifyJobFinished()
{
	if( 0 == InterlockedDecrement( &s_runningJobs ) )
	{
		if( nullptr != g_hwndDlg && IsWindow( g_hwndDlg ) )
			PostMessage( g_hwndDlg, WM_WAIT_CURSOR, FALSE, 0 );
	}
}