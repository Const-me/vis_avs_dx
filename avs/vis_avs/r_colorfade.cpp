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
// alphachannel safe 11/21/99
#include <windows.h>
#include <commctrl.h>
#include "r_defs.h"
#include "resource.h"

#include "timing.h"

#ifndef LASER

#define C_THISCLASS C_ColorFadeClass
#define MOD_NAME "Trans / Colorfade"

class C_THISCLASS : public C_RBASE2 {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	int enabled;
	int faders[ 3 ];
	int beatfaders[ 3 ];
	int faderpos[ 3 ];
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { faders[ 0 ] = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { faders[ 1 ] = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { faders[ 2 ] = GET_INT(); pos += 4; }
	memcpy( beatfaders, faders, 3 * sizeof( int ) );
	if( len - pos >= 4 ) { beatfaders[ 0 ] = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { beatfaders[ 1 ] = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { beatfaders[ 2 ] = GET_INT(); pos += 4; }
	memcpy( faderpos, faders, 3 * sizeof( int ) );
}
int C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( faders[ 0 ] ); pos += 4;
	PUT_INT( faders[ 1 ] ); pos += 4;
	PUT_INT( faders[ 2 ] ); pos += 4;
	PUT_INT( beatfaders[ 0 ] ); pos += 4;
	PUT_INT( beatfaders[ 1 ] ); pos += 4;
	PUT_INT( beatfaders[ 2 ] ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	enabled = 1;
	faders[ 0 ] = 8;
	faders[ 1 ] = faders[ 2 ] = -8;
	memcpy( beatfaders, faders, 3 * sizeof( int ) );
	memcpy( faderpos, faders, 3 * sizeof( int ) );

	CREATE_DX_EFFECT( enabled );
}

C_THISCLASS::~C_THISCLASS()
{ }

C_RBASE *R_ColorFade( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

static C_THISCLASS *g_this;

static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETPOS, 1, g_this->faders[ 0 ] + 32 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER2, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER2, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER2, TBM_SETPOS, 1, g_this->faders[ 1 ] + 32 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETPOS, 1, g_this->faders[ 2 ] + 32 );

		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETPOS, 1, g_this->beatfaders[ 0 ] + 32 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER5, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER5, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER5, TBM_SETPOS, 1, g_this->beatfaders[ 1 ] + 32 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER6, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER6, TBM_SETRANGEMAX, 0, 64 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER6, TBM_SETPOS, 1, g_this->beatfaders[ 2 ] + 32 );

		if( g_this->enabled & 1 ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_this->enabled & 2 ) CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );
		if( g_this->enabled & 4 ) CheckDlgButton( hwndDlg, IDC_CHECK3, BST_CHECKED );
		return 1;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_CHECK1:
		case IDC_CHECK2:
		case IDC_CHECK3:
			g_this->enabled = ( IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0 ) |
				( IsDlgButtonChecked( hwndDlg, IDC_CHECK2 ) ? 2 : 0 ) |
				( IsDlgButtonChecked( hwndDlg, IDC_CHECK3 ) ? 4 : 0 );
			return 0;

		}
		return 0;
	case WM_HSCROLL:
	{
		HWND swnd = (HWND)lParam;
		int t = (int)SendMessage( swnd, TBM_GETPOS, 0, 0 );
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER1 ) )
		{
			g_this->faders[ 0 ] = t - 32;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER2 ) )
		{
			g_this->faders[ 1 ] = t - 32;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER3 ) )
		{
			g_this->faders[ 2 ] = t - 32;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER4 ) )
		{
			g_this->beatfaders[ 0 ] = t - 32;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER5 ) )
		{
			g_this->beatfaders[ 1 ] = t - 32;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER6 ) )
		{
			g_this->beatfaders[ 2 ] = t - 32;
		}
	}
	return 0;
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_COLORFADE ), hwndParent, g_DlgProc );
}
#else
C_RBASE *R_ColorFade( char *desc ) { return NULL; }
#endif