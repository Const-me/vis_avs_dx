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
#include "r_defs.h"
#include "resource.h"

#ifndef LASER

#define C_THISCLASS C_NFClearClass
#define MOD_NAME "Render / OnBeat Clear"

class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	int nf, cf, df;

	int color, blend;
};


#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { color = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { nf = GET_INT(); pos += 4; }
}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	PUT_INT( color ); pos += 4;
	PUT_INT( blend ); pos += 4;
	PUT_INT( nf ); pos += 4;
	return pos;
}


C_THISCLASS::C_THISCLASS()
{
	cf = 0;
	nf = 1;
	df = 0;
	color = RGB( 255, 255, 255 );
	blend = 0;

	CREATE_DX_EFFECT( nf );
}

C_THISCLASS::~C_THISCLASS()
{
}

C_RBASE *R_NFClear( char *desc )
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
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT *)lParam;
		switch( di->CtlID )
		{
		case IDC_BUTTON1:
			GR_DrawColoredButton( di, g_this->color );
			break;
		}
	}
	return 0;
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMIN, 0, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMAX, 0, 100 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETPOS, 1, g_this->nf );

		if( g_this->blend ) CheckDlgButton( hwndDlg, IDC_BLEND, BST_CHECKED );
		return 1;

	case WM_HSCROLL:
	{
		HWND swnd = (HWND)lParam;
		int t = (int)SendMessage( swnd, TBM_GETPOS, 0, 0 );
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER1 ) )
		{
			g_this->nf = t;
		}
	}
	return 0;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_BUTTON1 )
		{
			GR_SelectColor( hwndDlg, &g_this->color );
			InvalidateRect( GetDlgItem( hwndDlg, LOWORD( wParam ) ), NULL, FALSE );
		}
		if( LOWORD( wParam ) == IDC_BLEND )
		{
			if( IsDlgButtonChecked( hwndDlg, IDC_BLEND ) )
				g_this->blend = 1;
			else g_this->blend = 0;
		}
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_NFC ), hwndParent, g_DlgProc );
}
#else
C_RBASE *R_NFClear( char *desc ) { return NULL; }
#endif