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
#include <windows.h>
#include <stdlib.h>
#include <vfw.h>
#include <commctrl.h>
#include "resource.h"
#include "r_defs.h"

#ifndef LASER

#define MOD_NAME "Trans / Mosaic"
#define C_THISCLASS C_MosaicClass

class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	float GET_FLOAT( unsigned char *data, int pos );
	void PUT_FLOAT( float f, unsigned char *data, int pos );
	void InitializeStars( int Start );
	void CreateStar( int A );
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );
	int enabled;
	int quality;
	int quality2;
	int blend;
	int blendavg;
	int onbeat;
	int durFrames;
	int nF;
	int thisQuality;
};

static C_THISCLASS *g_ConfigThis; // global configuration dialog pointer 
static HINSTANCE g_hDllInstance; // global DLL instance pointer (not needed in this example, but could be useful)

C_THISCLASS::~C_THISCLASS()
{ }

// configuration read/write
C_THISCLASS::C_THISCLASS() // set up default configuration
{
	enabled = 1;
	quality = 50;
	blend = 0;
	blendavg = 0;
	onbeat = 0;
	durFrames = 15;
	nF = 0;

	CREATE_DX_EFFECT( enabled );
}

#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))

void C_THISCLASS::load_config( unsigned char *data, int len ) // read configuration of max length "len" from data.
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { quality = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { quality2 = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blendavg = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { onbeat = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { durFrames = GET_INT(); pos += 4; }
}

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255

int  C_THISCLASS::save_config( unsigned char *data ) // write configuration to data, return length. config data should not exceed 64k.
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( quality ); pos += 4;
	PUT_INT( quality2 ); pos += 4;
	PUT_INT( blend ); pos += 4;
	PUT_INT( blendavg ); pos += 4;
	PUT_INT( onbeat ); pos += 4;
	PUT_INT( durFrames ); pos += 4;
	return pos;
}

// configuration dialog stuff
static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_QUALITY, TBM_SETTICFREQ, 10, 0 );
		SendDlgItemMessage( hwndDlg, IDC_QUALITY, TBM_SETRANGE, TRUE, MAKELONG( 1, 100 ) );
		SendDlgItemMessage( hwndDlg, IDC_QUALITY, TBM_SETPOS, TRUE, g_ConfigThis->quality );
		SendDlgItemMessage( hwndDlg, IDC_QUALITY2, TBM_SETTICFREQ, 10, 0 );
		SendDlgItemMessage( hwndDlg, IDC_QUALITY2, TBM_SETRANGE, TRUE, MAKELONG( 1, 100 ) );
		SendDlgItemMessage( hwndDlg, IDC_QUALITY2, TBM_SETPOS, TRUE, g_ConfigThis->quality2 );
		SendDlgItemMessage( hwndDlg, IDC_BEATDUR, TBM_SETTICFREQ, 10, 0 );
		SendDlgItemMessage( hwndDlg, IDC_BEATDUR, TBM_SETRANGE, TRUE, MAKELONG( 1, 100 ) );
		SendDlgItemMessage( hwndDlg, IDC_BEATDUR, TBM_SETPOS, TRUE, g_ConfigThis->durFrames );
		if( g_ConfigThis->enabled ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_ConfigThis->onbeat ) CheckDlgButton( hwndDlg, IDC_ONBEAT, BST_CHECKED );
		if( g_ConfigThis->blend ) CheckDlgButton( hwndDlg, IDC_ADDITIVE, BST_CHECKED );
		if( g_ConfigThis->blendavg ) CheckDlgButton( hwndDlg, IDC_5050, BST_CHECKED );
		if( !g_ConfigThis->blend && !g_ConfigThis->blendavg )
			CheckDlgButton( hwndDlg, IDC_REPLACE, BST_CHECKED );
		return 1;
	case WM_NOTIFY:
	{
		if( LOWORD( wParam ) == IDC_QUALITY2 )
			g_ConfigThis->quality2 = SendDlgItemMessage( hwndDlg, IDC_QUALITY2, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_BEATDUR )
			g_ConfigThis->durFrames = SendDlgItemMessage( hwndDlg, IDC_BEATDUR, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_QUALITY )
			g_ConfigThis->quality = SendDlgItemMessage( hwndDlg, IDC_QUALITY, TBM_GETPOS, 0, 0 );
	}
	return 0;
	case WM_COMMAND:
		if( ( LOWORD( wParam ) == IDC_CHECK1 ) ||
			( LOWORD( wParam ) == IDC_ONBEAT ) ||
			( LOWORD( wParam ) == IDC_ADDITIVE ) ||
			( LOWORD( wParam ) == IDC_REPLACE ) ||
			( LOWORD( wParam ) == IDC_5050 ) )
		{
			g_ConfigThis->enabled = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
			g_ConfigThis->onbeat = IsDlgButtonChecked( hwndDlg, IDC_ONBEAT ) ? 1 : 0;
			g_ConfigThis->blend = IsDlgButtonChecked( hwndDlg, IDC_ADDITIVE ) ? 1 : 0;
			g_ConfigThis->blendavg = IsDlgButtonChecked( hwndDlg, IDC_5050 ) ? 1 : 0;
		}
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent ) // return NULL if no config dialog possible
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_MOSAIC ), hwndParent, g_DlgProc );
}

// export stuff
C_RBASE *R_Mosaic( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

#else
C_RBASE *R_Mosaic( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	return NULL;
}
#endif