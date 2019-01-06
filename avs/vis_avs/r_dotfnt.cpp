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
#include "stdafx.h"
#include "resource.h"
#include "r_defs.h"

#define C_THISCLASS C_DotFountainClass
#define MOD_NAME "Render / Dot Fountain"

class C_THISCLASS : public C_RBASE
{
protected:
	float r;
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	int rotvel, angle;
	int colors[ 5 ];
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))

void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { rotvel = GET_INT(); pos += 4; }
	int x;
	for( x = 0; x < 5; x++ )
	{
		if( len - pos >= 4 ) { colors[ x ] = GET_INT(); pos += 4; }
	}
	if( len - pos >= 4 ) { angle = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { int rr = GET_INT(); pos += 4; r = rr / 32.0f; }
}

int C_THISCLASS::save_config( unsigned char *data )
{
	int rr;
	int pos = 0;
	int x;
	PUT_INT( rotvel ); pos += 4;
	for( x = 0; x < 5; x++ )
	{
		PUT_INT( colors[ x ] ); pos += 4;
	}
	PUT_INT( angle ); pos += 4;
	rr = (int)( r*32.0f );
	PUT_INT( rr ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	colors[ 0 ] = RGB( 24, 107, 28 ); // reverse BGR :)
	colors[ 1 ] = RGB( 35, 10, 255 );
	colors[ 2 ] = RGB( 116, 29, 42 );
	colors[ 3 ] = RGB( 217, 54, 144 );
	colors[ 4 ] = RGB( 255, 136, 107 );

	angle = -20;
	rotvel = 16;

	CREATE_DX_EFFECT( rotvel );
}

C_THISCLASS::~C_THISCLASS()
{
}


C_RBASE *R_DotFountain( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

static C_THISCLASS *g_this;

static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int *a = NULL;
	switch( uMsg )
	{
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_BUTTON1 )
		{
			g_this->rotvel = 0;
			SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETPOS, 1, 50 );
		}
		if( LOWORD( wParam ) >= IDC_C1 && LOWORD( wParam ) <= IDC_C5 ) {
			GR_SelectColor( hwndDlg, &g_this->colors[ IDC_C5 - LOWORD( wParam ) ] );
			InvalidateRect( GetDlgItem( hwndDlg, LOWORD( wParam ) ), NULL, FALSE );
		}
		return 0;
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT *)lParam;
		if( di->CtlID >= IDC_C1 && di->CtlID <= IDC_C5 )
		{
			GR_DrawColoredButton( di, g_this->colors[ IDC_C5 - di->CtlID ] );
		}
	}
	return 0;
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMAX, 0, 101 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETPOS, 1, g_this->rotvel + 50 );
		SendDlgItemMessage( hwndDlg, IDC_ANGLE, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_ANGLE, TBM_SETRANGEMAX, 0, 181 );
		SendDlgItemMessage( hwndDlg, IDC_ANGLE, TBM_SETPOS, 1, g_this->angle + 90 );

		return 1;

	case WM_HSCROLL:
	{
		HWND swnd = (HWND)lParam;
		int t = (int)SendMessage( swnd, TBM_GETPOS, 0, 0 );
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER1 ) )
		{
			g_this->rotvel = t - 50;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_ANGLE ) )
		{
			g_this->angle = t - 90;
		}
	}
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_DOTPLANE ), hwndParent, g_DlgProc );
}