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

#define MOD_NAME "Trans / Unique tone"
#define C_THISCLASS C_OnetoneClass

class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );
	int enabled;
	int invert;
	int color;
	int blend, blendavg;
};


static C_THISCLASS *g_ConfigThis; // global configuration dialog pointer 
static HINSTANCE g_hDllInstance; // global DLL instance pointer (not needed in this example, but could be useful)


C_THISCLASS::~C_THISCLASS()
{
}

// configuration read/write

C_THISCLASS::C_THISCLASS() // set up default configuration
{
	color = 0xFFFFFF;
	invert = 0;
	blend = 0;
	blendavg = 0;
	enabled = 1;
	CREATE_DX_EFFECT( enabled );
}

#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len ) // read configuration of max length "len" from data.
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { color = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blendavg = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { invert = GET_INT(); pos += 4; }
}

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
int  C_THISCLASS::save_config( unsigned char *data ) // write configuration to data, return length. config data should not exceed 64k.
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( color ); pos += 4;
	PUT_INT( blend ); pos += 4;
	PUT_INT( blendavg ); pos += 4;
	PUT_INT( invert ); pos += 4;
	return pos;
}

// configuration dialog stuff


static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		if( g_ConfigThis->enabled ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_ConfigThis->invert ) CheckDlgButton( hwndDlg, IDC_INVERT, BST_CHECKED );
		if( g_ConfigThis->blend ) CheckDlgButton( hwndDlg, IDC_ADDITIVE, BST_CHECKED );
		if( g_ConfigThis->blendavg ) CheckDlgButton( hwndDlg, IDC_5050, BST_CHECKED );
		if( !g_ConfigThis->blend && !g_ConfigThis->blendavg )
			CheckDlgButton( hwndDlg, IDC_REPLACE, BST_CHECKED );
		return 1;
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT *)lParam;
		if( di->CtlID == IDC_DEFCOL ) // paint nifty color button
		{
			int w = di->rcItem.right - di->rcItem.left;
			int _color = g_ConfigThis->color;
			_color = ( ( _color >> 16 ) & 0xff ) | ( _color & 0xff00 ) | ( ( _color << 16 ) & 0xff0000 );

			HPEN hPen, hOldPen;
			HBRUSH hBrush, hOldBrush;
			LOGBRUSH lb = { BS_SOLID, (COLORREF)_color, 0 };
			hPen = (HPEN)CreatePen( PS_SOLID, 0, _color );
			hBrush = CreateBrushIndirect( &lb );
			hOldPen = (HPEN)SelectObject( di->hDC, hPen );
			hOldBrush = (HBRUSH)SelectObject( di->hDC, hBrush );
			Rectangle( di->hDC, di->rcItem.left, di->rcItem.top, di->rcItem.right, di->rcItem.bottom );
			SelectObject( di->hDC, hOldPen );
			SelectObject( di->hDC, hOldBrush );
			DeleteObject( hBrush );
			DeleteObject( hPen );
		}
	}
	return 0;
	case WM_COMMAND:
		if( ( LOWORD( wParam ) == IDC_CHECK1 ) ||
			( LOWORD( wParam ) == IDC_ADDITIVE ) ||
			( LOWORD( wParam ) == IDC_REPLACE ) ||
			( LOWORD( wParam ) == IDC_5050 ) ||
			( LOWORD( wParam ) == IDC_INVERT ) )
		{
			g_ConfigThis->enabled = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
			g_ConfigThis->blend = IsDlgButtonChecked( hwndDlg, IDC_ADDITIVE ) ? 1 : 0;
			g_ConfigThis->blendavg = IsDlgButtonChecked( hwndDlg, IDC_5050 ) ? 1 : 0;
			g_ConfigThis->invert = IsDlgButtonChecked( hwndDlg, IDC_INVERT ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_DEFCOL ) // handle clicks to nifty color button
		{
			int *a = &( g_ConfigThis->color );
			static COLORREF custcolors[ 16 ];
			CHOOSECOLOR cs;
			cs.lStructSize = sizeof( cs );
			cs.hwndOwner = hwndDlg;
			cs.hInstance = 0;
			cs.rgbResult = ( ( *a >> 16 ) & 0xff ) | ( *a & 0xff00 ) | ( ( *a << 16 ) & 0xff0000 );
			cs.lpCustColors = custcolors;
			cs.Flags = CC_RGBINIT | CC_FULLOPEN;
			if( ChooseColor( &cs ) )
			{
				*a = ( ( cs.rgbResult >> 16 ) & 0xff ) | ( cs.rgbResult & 0xff00 ) | ( ( cs.rgbResult << 16 ) & 0xff0000 );
				g_ConfigThis->color = *a;
			}
			InvalidateRect( GetDlgItem( hwndDlg, IDC_DEFCOL ), NULL, TRUE );
		}
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent ) // return NULL if no config dialog possible
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_ONETONE ), hwndParent, g_DlgProc );
}

// export stuff
C_RBASE *R_Onetone( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

#else
C_RBASE *R_Onetone( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	return NULL;
}
#endif