#include "stdafx.h"
#include "cfgwnd.h"
#include "wnd.h"
#include "avs_eelif.h"
#include "bpm.h"
#include "render.h"
#include "VIS.H"
#include "SourceBuffer.h"
#include <Threads/GuiThread.h>
#include "wa_ipc.h"

#define CHECK( hr, msg )        {  const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }
#define FAIL_LAST_WIN32( msg )  {  const HRESULT __hr = getLastHr(); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }

extern SourceBuffer g_sourceBuffer;

static CStringA s_winampIniPath;

static HRESULT queryWinampIniPath( winampVisModule * this_mod )
{
	const HWND wndWinamp = this_mod->hwndParent;
	if( nullptr == wndWinamp || !IsWindow( wndWinamp ) )
	{
		logError( "getWinampIniPath() failed, no winamp window" );
		return E_FAIL;
	}

	const LRESULT lRes = SendMessageA( this_mod->hwndParent, WM_WA_IPC, 0, IPC_GETINIFILE );
	if( 0 == lRes )
	{
		logError( "getWinampIniPath() failed, IPC_GETINIFILE returned null pointer" );
		return E_FAIL;
	}
	s_winampIniPath = (const char*)lRes;
	return S_OK;
}

const CStringA& getWinampIniPath() { return s_winampIniPath; }
#define SILENT_CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) return __hr; }

HRESULT GuiThread::startup()
{
	// Query the winamp.ini path and store in a static variable.
	// This should prevent rare hang on shutdown where SendMessage( WM_WA_IPC, IPC_GETINIFILE ); never returns because the target is already dead.
	SILENT_CHECK( queryWinampIniPath( this_mod ) );

	AVS_EEL_IF_init();

	if( Wnd_Init( this_mod ) )
		return E_FAIL;

	g_sourceBuffer.buildLogTable();

	initBpm();

	Render_Init( g_hInstance );

	CfgWnd_Create( this_mod );

	SetForegroundWindow( g_hwnd );
	SetFocus( g_hwnd );

	return S_OK;
}

void GuiThread::shutdown()
{
	logShutdown( "GuiThread::shutdown" );
	int wasOpen = cfg_cfgwnd_open;
	CfgWnd_Destroy();
	cfg_cfgwnd_open = wasOpen;
	Render_Quit( this_mod->hDllInstance );
	Wnd_Quit();
	AVS_EEL_IF_quit();
}

void GuiThread::configOpen()
{
	CfgWnd_Create( this_mod );
}

void GuiThread::configClose()
{
	CfgWnd_Destroy();
}