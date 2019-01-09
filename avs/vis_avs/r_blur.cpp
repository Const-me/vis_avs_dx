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

#define C_THISCLASS C_BlurClass
#define MOD_NAME "Trans / Blur"

static const int zero = 0;

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
	int roundmode;
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { roundmode = GET_INT(); pos += 4; }
	else roundmode = 0;
}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	PUT_INT( enabled ); pos += 4;
	PUT_INT( roundmode ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	roundmode = 0;
	enabled = 1;
	CREATE_DX_EFFECT( enabled );
}

C_THISCLASS::~C_THISCLASS()
{ }

C_RBASE *R_Blur( char *desc )
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
		if( g_this->enabled == 2 ) CheckDlgButton( hwndDlg, IDC_RADIO3, BST_CHECKED );
		else if( g_this->enabled == 3 ) CheckDlgButton( hwndDlg, IDC_RADIO4, BST_CHECKED );
		else if( g_this->enabled ) CheckDlgButton( hwndDlg, IDC_RADIO2, BST_CHECKED );
		else CheckDlgButton( hwndDlg, IDC_RADIO1, BST_CHECKED );
		if( g_this->roundmode == 0 ) CheckDlgButton( hwndDlg, IDC_ROUNDDOWN, BST_CHECKED );
		else CheckDlgButton( hwndDlg, IDC_ROUNDUP, BST_CHECKED );
		ShowWindow( GetDlgItem( hwndDlg, IDC_ROUNDUP ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_ROUNDDOWN ), SW_HIDE );
		return 1;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_RADIO1 )
			if( IsDlgButtonChecked( hwndDlg, IDC_RADIO1 ) )
				g_this->enabled = 0;
		if( LOWORD( wParam ) == IDC_RADIO2 )
			if( IsDlgButtonChecked( hwndDlg, IDC_RADIO2 ) )
				g_this->enabled = 1;
		if( LOWORD( wParam ) == IDC_RADIO3 )
			if( IsDlgButtonChecked( hwndDlg, IDC_RADIO3 ) )
				g_this->enabled = 2;
		if( LOWORD( wParam ) == IDC_RADIO4 )
			if( IsDlgButtonChecked( hwndDlg, IDC_RADIO4 ) )
				g_this->enabled = 3;
		if( LOWORD( wParam ) == IDC_ROUNDUP )
			if( IsDlgButtonChecked( hwndDlg, IDC_ROUNDUP ) )
				g_this->roundmode = 1;
		if( LOWORD( wParam ) == IDC_ROUNDDOWN )
			if( IsDlgButtonChecked( hwndDlg, IDC_ROUNDDOWN ) )
				g_this->roundmode = 0;
		return 0;
		return 0;
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_BLUR ), hwndParent, g_DlgProc );
}

#else 
C_RBASE *R_Blur( char *desc ) { return NULL; }
#endif