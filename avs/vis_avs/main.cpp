/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

  * Neither the name of Nullsoft nor the names of its contributors may be used to
	endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "stdafx.h"
#include "draw.h"
#include "wnd.h"
#include "r_defs.h"
#include "render.h"
#include "vis.h"
#include "cfgwnd.h"
#include "resource.h"
#include <stdio.h>
#include <shellapi.h>

#ifdef WA3_COMPONENT
#include "wasabicfg.h"
#include "../studio/studio/api.h"
#endif
#include "avs_eelif.h"
#include <Threads/threadsApi.h>
#include "SourceBuffer.h"

extern void GetClientRect_adj( HWND hwnd, RECT *r );

HINSTANCE g_hInstance;

char *verstr =
#ifndef LASER
"Advanced Visualization Studio"
#else
"AVS/Laser"
#endif
" v2.81d"
;

// DWORD WINAPI RenderThread( LPVOID a );

static void config( struct winampVisModule *this_mod );
static int init( struct winampVisModule *this_mod );
static int render( struct winampVisModule *this_mod );
static void quit( struct winampVisModule *this_mod );

#ifndef WA3_COMPONENT
SourceBuffer g_sourceBuffer;

static winampVisModule *getModule( int which );
static winampVisHeader hdr = { VIS_HDRVER, verstr, getModule };

extern "C" {
	__declspec( dllexport ) winampVisHeader* winampVisGetHeader()
	{
		return &hdr;
	}
}

static winampVisModule *getModule( int which )
{
	static winampVisModule mod =
	{
#ifdef LASER
		"Advanced Visualization Studio/Laser",
#else
		"Advanced Visualization Studio",
#endif
		NULL,	// hwndParent
		NULL,	// hDllInstance
		0,		// sRate
		0,		// nCh
		1000 / 70,		// latencyMS
		1000 / 70,// delayMS
		2,		// spectrumNch
		2,		// waveformNch
		{ 0, },	// spectrumData
		{ 0, },	// waveformData
		config,
		init,
		render,
		quit
	};
	if( which == 0 ) return &mod;
	return 0;
}
#endif

BOOL CALLBACK aboutProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SetDlgItemText( hwndDlg, IDC_VERSTR, verstr );

		return 1;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDOK: case IDCANCEL:
			EndDialog( hwndDlg, 0 );
			return 0;
		}
		return 0;
	}
	return 0;
}

BOOL __stdcall aboutProcDx( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_COMMAND )
	{
		const WORD id = LOWORD( wParam );
		switch( id )
		{
		case IDOK: case IDCANCEL:
			EndDialog( hwndDlg, 0 );
		}
		return FALSE;
	}
	if( uMsg == WM_NOTIFY && wParam == IDC_SYSLINK1 )
	{
		const NMHDR *pNmh = (const NMHDR *)lParam;
		if( pNmh->code == NM_CLICK )
			ShellExecuteW( 0, 0, L"http://const.me/", 0, 0, SW_SHOW );
		return FALSE;
	}
	return FALSE;
}

static void config( struct winampVisModule *this_mod )
{
	if( !g_hwnd || !IsWindow( g_hwnd ) )
	{
		DialogBox( this_mod->hDllInstance, MAKEINTRESOURCE( IDD_DIALOG2 ), this_mod->hwndParent, aboutProc );
	}
	else
	{
		SendMessage( g_hwnd, WM_USER + 33, 0, 0 );
	}
}

char g_path[ 1024 ];

extern void previous_preset( HWND hwnd );
extern void next_preset( HWND hwnd );
extern void random_preset( HWND hwnd );

#if 0//syntax highlighting
HINSTANCE hRich;
#endif

static int init( struct winampVisModule *this_mod )
{
	FILETIME ft;
#if 0//syntax highlighting
	if( !hRich ) hRich = LoadLibrary( "RICHED32.dll" );
#endif
	GetSystemTimeAsFileTime( &ft );
	srand( ft.dwLowDateTime | ft.dwHighDateTime^GetCurrentThreadId() );
	g_hInstance = this_mod->hDllInstance;
#ifdef WA3_COMPONENT
	GetModuleFileName( GetModuleHandle( NULL ), g_path, MAX_PATH );
#else
	GetModuleFileName( g_hInstance, g_path, MAX_PATH );
#endif
	char *p = g_path + strlen( g_path );
	while( p > g_path && *p != '\\' ) p--;
	*p = 0;

#ifdef WA2_EMBED
	if( SendMessage( this_mod->hwndParent, WM_USER, 0, 0 ) < 0x2900 )
	{
		MessageBox( this_mod->hwndParent, "This version of AVS requires Winamp 2.9+", "AVS ERROR", MB_OK | MB_ICONSTOP );
		return 1;
	}
#endif

#ifdef WA3_COMPONENT
	strcat( g_path, "\\wacs\\data" );
#endif

#ifdef LASER
	strcat( g_path, "\\avs_laser" );
#else
	strcat( g_path, "\\avs" );
#endif
	CreateDirectory( g_path, NULL );

	return FAILED( avsThreadsStart( this_mod ) ) ? 1 : 0;
}

static int render( struct winampVisModule *this_mod )
{
	if( avsThreadsShuttingDown() )
		return 1;

	return g_sourceBuffer.update( this_mod );
}

static void quit( struct winampVisModule *this_mod )
{
	avsThreadsStop();
}

#ifdef WA3_COMPONENT
static winampVisModule dummyMod;

void init3( void )
{
	extern HWND g_wndparent;
	dummyMod.hwndParent = g_wndparent;
	dummyMod.hDllInstance = g_hInstance;
	init( &dummyMod );
}

void quit3( void )
{
	extern HWND last_parent;
	if( last_parent )
	{
		ShowWindow( GetParent( last_parent ), SW_SHOWNA );
	}
	quit( &dummyMod );
}
#endif

HRESULT RenderThread::startup()
{
	FILETIME ft;
	GetSystemTimeAsFileTime( &ft );
	srand( ft.dwLowDateTime ^ ft.dwHighDateTime ^ GetCurrentThreadId() );
	return S_OK;
}

void RenderThread::renderFrame()
{
	alignas( 16 ) uint16_t vis_data[ 2 ][ 2 ][ 576 ];
	bool beat = false;
	g_sourceBuffer.copy( vis_data, beat );
	g_render_transition->render( vis_data, beat, nullptr, nullptr, 0, 0 );
}