#include "stdafx.h"
#include "guiThread.h"
#include "EventSlim.hpp"

#include "cfgwnd.h"
#include "wnd.h"
#include "avs_eelif.h"
#include "bpm.h"
#include "render.h"
#include "main.h"
#include "VIS.H"

constexpr DWORD msQuitTimeout = 1000;

#define CHECK( hr, msg )        {  const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }
#define FAIL_LAST_WIN32( msg )  {  const HRESULT __hr = getLastHr(); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }

extern unsigned char g_logtab[ 256 ];
extern CHandle g_hThread;
extern volatile int g_ThreadQuit;

HRESULT shutdownThread();

extern void onRenderThreadShuttingDown( bool renderThread );

namespace guiThread
{
	DWORD threadId = 0;
	CHandle hThread;
	CHandle eventThreadReady;

	constexpr UINT WM_CONFIG_OPEN = WM_APP + 11;
	constexpr UINT WM_CONFIG_CLOSE = WM_APP + 12;

	static HRESULT __stdcall threadProc( winampVisModule * this_mod );

	HRESULT start( winampVisModule *this_mod )
	{
		assert( !hThread );

		eventThreadReady.Attach( ::CreateEvent( nullptr, TRUE, FALSE, nullptr ) );

		hThread.Attach( ::CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)&threadProc, this_mod, 0, &threadId ) );
		if( !hThread )
			FAIL_LAST_WIN32( "Error creating an AVS GUI thread" );

		const HANDLE handles[ 2 ] = { eventThreadReady, hThread };
		const DWORD res = WaitForMultipleObjects( 2, handles, FALSE, 2500 );
		eventThreadReady.Close();

		switch( res )
		{
		case WAIT_OBJECT_0:
			return S_OK;
		case WAIT_OBJECT_0 + 1:
		{
			DWORD ec;
			GetExitCodeThread( hThread, &ec );
			return HRESULT( ec );
		}
		case WAIT_TIMEOUT:
			return CondVar::E_TIMEOUT;
		}
		return getLastHr();
	}

	HRESULT startup( winampVisModule * this_mod )
	{
		AVS_EEL_IF_init();

		if( Wnd_Init( this_mod ) )
			return E_FAIL;

		{
			int x;
			for( x = 0; x < 256; x++ )
			{
				double a = log( x*60.0 / 255.0 + 1.0 ) / log( 60.0 );
				int t = (int)( a*255.0 );
				if( t < 0 )t = 0;
				if( t > 255 )t = 255;
				g_logtab[ x ] = (unsigned char)t;
			}
		}

		initBpm();

		Render_Init( g_hInstance );

		CfgWnd_Create( this_mod );

		DWORD tid;
		g_hThread.Attach( CreateThread( nullptr, 0, RenderThread, 0, 0, &tid ) );
		if( !g_hThread )
			return getLastHr();

		main_setRenderThreadPriority();
		SetForegroundWindow( g_hwnd );
		SetFocus( g_hwnd );

		return S_OK;
	}

	void shutdown( winampVisModule * this_mod )
	{
		// Ask render thread to quit gracefully
		shutdownThread();

		g_ThreadQuit = 1;
		if( WaitForSingleObject( g_hThread, 10000 ) != WAIT_OBJECT_0 )
		{
			logError( "error waiting for thread to quit" );
			// DS( "Terminated thread (BAD!)\n" );
			// MessageBox(NULL,"error waiting for thread to quit","a",MB_TASKMODAL);
			TerminateThread( g_hThread, 0 );
		}
		g_hThread.Close();
		CfgWnd_Destroy();
		Render_Quit( this_mod->hDllInstance );
		Wnd_Quit();
		AVS_EEL_IF_quit();
		onRenderThreadShuttingDown( false );
		threadId = 0;
	}

	bool handleThreadMessage( winampVisModule * this_mod, const MSG& msg )
	{
		switch( msg.message )
		{
		case WM_CONFIG_OPEN:
			CfgWnd_Create( this_mod );
			return true;
		case WM_CONFIG_CLOSE:
			CfgWnd_Destroy();
			return true;
		}
		return false;
	}

	static HRESULT __stdcall threadProc( winampVisModule * this_mod )
	{
		CHECK( startup( this_mod ), "Error initializing AVS thread" );
		SetEvent( eventThreadReady );

		while( true )
		{
			MSG msg;
			if( !GetMessage( &msg, NULL, 0, 0 ) )
				break;	// WM_QUIT

			if( nullptr == msg.hwnd )
				if( handleThreadMessage( this_mod, msg ) )
					continue;

			if( IsDialogMessage( g_hwndDlg, &msg ) )
				continue;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		shutdown( this_mod );
		return S_OK;
	}

	HRESULT stop()
	{
		if( 0 == threadId )
			return S_FALSE;
		PostThreadMessage( threadId, WM_QUIT, 0, 0 );
		if( WAIT_OBJECT_0 == WaitForSingleObject( hThread, msQuitTimeout ) )
			return S_OK;
		const HRESULT hr =  getLastHr();
		if( hThread )
		{
			TerminateThread( hThread, E_FAIL );
			hThread.Close();
		}
		return hr;
	}

	HRESULT openConfig()
	{
		if( 0 == threadId )
			return S_FALSE;
		PostThreadMessage( threadId, WM_CONFIG_OPEN, 0, 0 );
		return S_OK;
	}
	HRESULT closeConfig()
	{
		if( 0 == threadId )
			return S_FALSE;
		PostThreadMessage( threadId, WM_CONFIG_CLOSE, 0, 0 );
		return S_OK;
	}
}