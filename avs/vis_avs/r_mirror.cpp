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
#include <commctrl.h>
#include "resource.h"
#include "r_defs.h"

#ifndef LASER

#define MOD_NAME "Trans / Mirror"
#define C_THISCLASS C_MirrorClass


class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS() { }
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	int enabled;
	int mode;
	int onbeat;
	int rbeat;
	int smooth;
	int slower;
};


static C_THISCLASS *g_ConfigThis; // global configuration dialog pointer 
static HINSTANCE g_hDllInstance; // global DLL instance pointer (not needed in this example, but could be useful)

#define HORIZONTAL1 1
#define HORIZONTAL2 2
#define VERTICAL1   4
#define VERTICAL2   8

#define D_HORIZONTAL1 0
#define D_HORIZONTAL2 8
#define D_VERTICAL1   16
#define D_VERTICAL2   24
#define M_HORIZONTAL1 0xFF
#define M_HORIZONTAL2 0xFF00
#define M_VERTICAL1   0xFF0000
#define M_VERTICAL2   0xFF000000

// configuration read/write

C_THISCLASS::C_THISCLASS() // set up default configuration
{
	enabled = 1;
	onbeat = 0;
	smooth = 0;
	slower = 4;
	mode = HORIZONTAL1;

	CREATE_DX_EFFECT( enabled );
}


#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len ) // read configuration of max length "len" from data.
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { mode = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { onbeat = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { smooth = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { slower = GET_INT(); pos += 4; }
}

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
int  C_THISCLASS::save_config( unsigned char *data ) // write configuration to data, return length. config data should not exceed 64k.
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( mode ); pos += 4;
	PUT_INT( onbeat ); pos += 4;
	PUT_INT( smooth ); pos += 4;
	PUT_INT( slower ); pos += 4;
	return pos;
}

int getMode( HWND hwndDlg )
{
	int a;
	a = IsDlgButtonChecked( hwndDlg, IDC_HORIZONTAL1 ) ? HORIZONTAL1 : 0;
	a |= IsDlgButtonChecked( hwndDlg, IDC_HORIZONTAL2 ) ? HORIZONTAL2 : 0;
	a |= IsDlgButtonChecked( hwndDlg, IDC_VERTICAL1 ) ? VERTICAL1 : 0;
	a |= IsDlgButtonChecked( hwndDlg, IDC_VERTICAL2 ) ? VERTICAL2 : 0;
	return a;
}

// configuration dialog stuff
static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_SLOWER, TBM_SETTICFREQ, 1, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLOWER, TBM_SETRANGE, TRUE, MAKELONG( 1, 16 ) );
		SendDlgItemMessage( hwndDlg, IDC_SLOWER, TBM_SETPOS, TRUE, g_ConfigThis->slower );
		if( g_ConfigThis->enabled ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_ConfigThis->smooth ) CheckDlgButton( hwndDlg, IDC_SMOOTH, BST_CHECKED );
		if( g_ConfigThis->mode & VERTICAL1 )
			CheckDlgButton( hwndDlg, IDC_VERTICAL1, BST_CHECKED );
		if( g_ConfigThis->mode & VERTICAL2 )
			CheckDlgButton( hwndDlg, IDC_VERTICAL2, BST_CHECKED );
		if( g_ConfigThis->mode & HORIZONTAL1 )
			CheckDlgButton( hwndDlg, IDC_HORIZONTAL1, BST_CHECKED );
		if( g_ConfigThis->mode & HORIZONTAL2 )
			CheckDlgButton( hwndDlg, IDC_HORIZONTAL2, BST_CHECKED );
		if( g_ConfigThis->onbeat )
			CheckDlgButton( hwndDlg, IDC_ONBEAT, BST_CHECKED );
		else
			CheckDlgButton( hwndDlg, IDC_STAT, BST_CHECKED );
		return 1;
	case WM_NOTIFY:
	{
		if( LOWORD( wParam ) == IDC_SLOWER )
			g_ConfigThis->slower = SendDlgItemMessage( hwndDlg, IDC_SLOWER, TBM_GETPOS, 0, 0 );
		return 0;
	}
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_CHECK1 )
		{
			g_ConfigThis->enabled = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_HORIZONTAL1 )
		{
			g_ConfigThis->mode = getMode( hwndDlg );
			if( ( g_ConfigThis->mode & HORIZONTAL1 ) && ( g_ConfigThis->mode & HORIZONTAL2 ) && !( g_ConfigThis->onbeat ) )
			{
				CheckDlgButton( hwndDlg, IDC_HORIZONTAL2, BST_UNCHECKED );
				g_ConfigThis->mode = getMode( hwndDlg );
			}
		}
		if( LOWORD( wParam ) == IDC_HORIZONTAL2 )
		{
			g_ConfigThis->mode = getMode( hwndDlg );
			if( ( g_ConfigThis->mode & HORIZONTAL1 ) && ( g_ConfigThis->mode & HORIZONTAL2 ) && !( g_ConfigThis->onbeat ) )
			{
				CheckDlgButton( hwndDlg, IDC_HORIZONTAL1, BST_UNCHECKED );
				g_ConfigThis->mode = getMode( hwndDlg );
			}
		}
		if( LOWORD( wParam ) == IDC_VERTICAL1 )
		{
			g_ConfigThis->mode = getMode( hwndDlg );
			if( ( g_ConfigThis->mode & VERTICAL1 ) && ( g_ConfigThis->mode & VERTICAL2 ) && !( g_ConfigThis->onbeat ) )
			{
				CheckDlgButton( hwndDlg, IDC_VERTICAL2, BST_UNCHECKED );
				g_ConfigThis->mode = getMode( hwndDlg );
			}
		}
		if( LOWORD( wParam ) == IDC_VERTICAL2 )
		{
			g_ConfigThis->mode = getMode( hwndDlg );
			if( ( g_ConfigThis->mode & VERTICAL1 ) && ( g_ConfigThis->mode & VERTICAL2 ) && !( g_ConfigThis->onbeat ) )
			{
				CheckDlgButton( hwndDlg, IDC_VERTICAL1, BST_UNCHECKED );
				g_ConfigThis->mode = getMode( hwndDlg );
			}
		}
		if( LOWORD( wParam ) == IDC_STAT || LOWORD( wParam ) == IDC_ONBEAT )
		{
			g_ConfigThis->onbeat = IsDlgButtonChecked( hwndDlg, IDC_ONBEAT ) ? 1 : 0;
			if( !( g_ConfigThis->onbeat ) )
			{
				if( ( g_ConfigThis->mode & HORIZONTAL1 ) && ( g_ConfigThis->mode & HORIZONTAL2 ) )
					CheckDlgButton( hwndDlg, IDC_HORIZONTAL2, BST_UNCHECKED );
				if( ( g_ConfigThis->mode & VERTICAL1 ) && ( g_ConfigThis->mode & VERTICAL2 ) )
					CheckDlgButton( hwndDlg, IDC_VERTICAL2, BST_UNCHECKED );
				g_ConfigThis->mode = getMode( hwndDlg );
			}
		}
		if( LOWORD( wParam ) == IDC_SMOOTH )
			g_ConfigThis->smooth = IsDlgButtonChecked( hwndDlg, IDC_SMOOTH ) ? 1 : 0;
		return 0;
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent ) // return NULL if no config dialog possible
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_MIRROR ), hwndParent, g_DlgProc );
}

// export stuff
C_RBASE *R_Mirror( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

#else
C_RBASE *R_Mirror( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	return NULL;
}
#endif