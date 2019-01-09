#include "stdafx.h"
#include "cfgwnd.h"
#include "wnd.h"
#include "avs_eelif.h"
#include "bpm.h"
#include "render.h"
#include "main.h"
#include "VIS.H"
#include "SourceBuffer.h"
#include <Threads/GuiThread.h>

#define CHECK( hr, msg )        {  const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }
#define FAIL_LAST_WIN32( msg )  {  const HRESULT __hr = getLastHr(); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }

extern CHandle g_hThread;
extern SourceBuffer g_sourceBuffer;

HRESULT GuiThread::startup()
{
	AVS_EEL_IF_init();

	if( Wnd_Init( this_mod ) )
		return E_FAIL;

	g_sourceBuffer.buildLogTable();

	initBpm();

	Render_Init( g_hInstance );

	CfgWnd_Create( this_mod );

	/* DWORD tid;
	g_hThread.Attach( CreateThread( nullptr, 0, RenderThread, 0, 0, &tid ) );
	if( !g_hThread )
		return getLastHr(); */

	main_setRenderThreadPriority();
	SetForegroundWindow( g_hwnd );
	SetFocus( g_hwnd );

	return S_OK;
}

void GuiThread::shutdown()
{
	CfgWnd_Destroy();
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