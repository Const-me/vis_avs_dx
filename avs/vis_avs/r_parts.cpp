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
#include <math.h>
#include "r_defs.h"

#include "resource.h"

#ifndef LASER

#define C_THISCLASS C_BPartsClass
#define MOD_NAME "Render / Moving Particle"

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
	int colors;
	int maxdist, size, size2;
	int blend;

	int s_pos;

	double c[ 2 ];
	double v[ 2 ];
	double p[ 2 ];
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { colors = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { maxdist = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { size = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { size2 = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	s_pos = size;
}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( colors ); pos += 4;
	PUT_INT( maxdist ); pos += 4;
	PUT_INT( size ); pos += 4;
	PUT_INT( size2 ); pos += 4;
	PUT_INT( blend ); pos += 4;
	return pos;
}



C_THISCLASS::C_THISCLASS()
{
	blend = 1;
	size = size2 = s_pos = 8;
	maxdist = 16;
	enabled = 1;
	colors = RGB( 255, 255, 255 );
	c[ 0 ] = c[ 1 ] = 0.0f;
	v[ 0 ] = -0.01551;
	v[ 1 ] = 0.0;

	p[ 0 ] = -0.6;
	p[ 1 ] = 0.3;

	CREATE_DX_EFFECT( enabled );
}

C_THISCLASS::~C_THISCLASS()
{
}

C_RBASE *R_Parts( char *desc )
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
		case IDC_LC:
			GR_DrawColoredButton( di, g_this->colors );
			break;
		}
	}
	return 0;
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMIN, 0, 1 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETRANGEMAX, 0, 32 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER1, TBM_SETPOS, 1, g_this->maxdist );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETRANGEMIN, 0, 1 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETRANGEMAX, 0, 128 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER3, TBM_SETPOS, 1, g_this->size );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETRANGEMIN, 0, 1 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETRANGEMAX, 0, 128 );
		SendDlgItemMessage( hwndDlg, IDC_SLIDER4, TBM_SETPOS, 1, g_this->size2 );
		if( g_this->enabled & 1 ) CheckDlgButton( hwndDlg, IDC_LEFT, BST_CHECKED );
		if( g_this->enabled & 2 ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );

		if( g_this->blend == 1 ) CheckDlgButton( hwndDlg, IDC_RADIO2, BST_CHECKED );
		else if( g_this->blend == 2 ) CheckDlgButton( hwndDlg, IDC_RADIO3, BST_CHECKED );
		else if( g_this->blend == 3 ) CheckDlgButton( hwndDlg, IDC_RADIO4, BST_CHECKED );
		else CheckDlgButton( hwndDlg, IDC_RADIO1, BST_CHECKED );

		return 1;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_RADIO1:
		case IDC_RADIO2:
		case IDC_RADIO3:
		case IDC_RADIO4:
			if( IsDlgButtonChecked( hwndDlg, IDC_RADIO1 ) ) g_this->blend = 0;
			else if( IsDlgButtonChecked( hwndDlg, IDC_RADIO2 ) ) g_this->blend = 1;
			else if( IsDlgButtonChecked( hwndDlg, IDC_RADIO3 ) ) g_this->blend = 2;
			else if( IsDlgButtonChecked( hwndDlg, IDC_RADIO4 ) ) g_this->blend = 3;
			break;
		case IDC_LEFT:
			g_this->enabled &= ~1;
			g_this->enabled |= IsDlgButtonChecked( hwndDlg, IDC_LEFT ) ? 1 : 0;
			return 0;
		case IDC_CHECK1:
			g_this->enabled &= ~2;
			g_this->enabled |= IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 2 : 0;
			return 0;
		case IDC_LC:
			if( !a ) a = &g_this->colors;
			GR_SelectColor( hwndDlg, a );
			InvalidateRect( GetDlgItem( hwndDlg, LOWORD( wParam ) ), NULL, FALSE );
			return 0;


		}
		return 0;
	case WM_HSCROLL:
	{
		HWND swnd = (HWND)lParam;
		int t = (int)SendMessage( swnd, TBM_GETPOS, 0, 0 );
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER1 ) )
		{
			g_this->maxdist = t;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER3 ) )
		{
			g_this->s_pos = g_this->size = t;
		}
		if( swnd == GetDlgItem( hwndDlg, IDC_SLIDER4 ) )
		{
			g_this->s_pos = g_this->size2 = t;
		}
	}
	return 0;


	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_PARTS ), hwndParent, g_DlgProc );
}

#else
C_RBASE *R_Parts( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	return NULL;
}
#endif