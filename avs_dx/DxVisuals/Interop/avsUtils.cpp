#include "stdafx.h"
#include "avsUtils.h"
#include <atlpath.h>

CStringA getPrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpFileName )
{
	CStringA res;
	DWORD cc = GetPrivateProfileStringA( lpAppName, lpKeyName, "", res.GetBufferSetLength( MAX_PATH ), MAX_PATH, lpFileName );
	res.ReleaseBufferSetLength( cc );
	return res;
}

constexpr int MSGF_SLEEPMSG = 0x5300;

HRESULT msgWaitForSingleObject( HANDLE h, DWORD ms )
{
	// https://blogs.msdn.microsoft.com/oldnewthing/20060126-00/?p=32513/
	while( true )
	{
		const DWORD started = GetTickCount();
		const DWORD res = MsgWaitForMultipleObjectsEx( 1, &h, ms, QS_ALLINPUT, MWMO_INPUTAVAILABLE );
		if( WAIT_OBJECT_0 == res )
			return S_OK;
		if( WAIT_OBJECT_0 + 1 == res )
		{
			MSG msg;
			while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				if( msg.message == WM_QUIT )
				{
					PostQuitMessage( (int)msg.wParam );
					return S_FALSE; // Abandoned due to WM_QUIT
				}
				if( !CallMsgFilter( &msg, MSGF_SLEEPMSG ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			const DWORD elapsed = GetTickCount() - started;	//< This works OK even when DWORD overflows: https://codereview.stackexchange.com/a/129670/46194
			if( elapsed > ms )
				return E_TIMEOUT;
			ms -= elapsed;
			continue;
		}
		if( WAIT_TIMEOUT == res )
			return E_TIMEOUT;
		HRESULT hr = getLastHr();
		if( FAILED( hr ) )
			return hr;
		return E_UNEXPECTED;	// Unasked IO_COMPLETION or ABANDONED?
	}
}

void logLoadingPreset( const char* path )
{
	CPathA dir = path;
	dir.RemoveFileSpec();
	dir.RemoveFileSpec();
	logInfo( "Loading preset \"%s\"", path + dir.m_strPath.GetLength() + 1 );
}