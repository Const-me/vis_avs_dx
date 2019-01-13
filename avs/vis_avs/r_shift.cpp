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
#include "r_defs.h"
#include "resource.h"
#include "avs_eelif.h"

#define C_THISCLASS C_ShiftClass
#define MOD_NAME "Trans / Dynamic Shift"

class C_THISCLASS : public C_RBASE
{
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	RString effect_exp[ 3 ];
	int blend, subpixel;
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( data[ pos ] == 1 )
	{
		pos++;
		load_string( effect_exp[ 0 ], data, pos, len );
		load_string( effect_exp[ 1 ], data, pos, len );
		load_string( effect_exp[ 2 ], data, pos, len );
	}
	else
	{
		char buf[ 769 ];
		if( len - pos >= 768 )
		{
			memcpy( buf, data + pos, 768 );
			pos += 768;
			buf[ 768 ] = 0;
			effect_exp[ 2 ].assign( buf + 512 );
			buf[ 512 ] = 0;
			effect_exp[ 1 ].assign( buf + 256 );
			buf[ 256 ] = 0;
			effect_exp[ 0 ].assign( buf );
		}
	}
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { subpixel = GET_INT(); pos += 4; }

}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	data[ pos++ ] = 1;
	save_string( data, pos, effect_exp[ 0 ] );
	save_string( data, pos, effect_exp[ 1 ] );
	save_string( data, pos, effect_exp[ 2 ] );
	PUT_INT( blend ); pos += 4;
	PUT_INT( subpixel ); pos += 4;
	return pos;
}



C_THISCLASS::C_THISCLASS()
{
	effect_exp[ 0 ].assign( "d=0;" ); // init
	effect_exp[ 1 ].assign( "x=sin(d)*1.4; y=1.4*cos(d); d=d+0.01;" ); // frame
	effect_exp[ 2 ].assign( "d=d+2.0" );
	blend = 0;
	subpixel = 1;

	CREATE_DX_EFFECT( effect_exp[ 0 ] );
}

C_THISCLASS::~C_THISCLASS()
{ }


C_RBASE *R_Shift( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

static C_THISCLASS *g_this;
static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int isstart;
	switch( uMsg )
	{
	case WM_INITDIALOG:
		isstart = 1;
		SetDlgItemText( hwndDlg, IDC_EDIT1, g_this->effect_exp[ 0 ].get() );
		SetDlgItemText( hwndDlg, IDC_EDIT2, g_this->effect_exp[ 1 ].get() );
		SetDlgItemText( hwndDlg, IDC_EDIT3, g_this->effect_exp[ 2 ].get() );
		isstart = 0;
		if( g_this->blend )
			CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_this->subpixel )
			CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );
		return 1;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_HELPBTN )
		{
			char *text = "Dynamic Shift\0"
				"better Dynamic shift help goes here (send me some :)\r\n";
			"Variables:\r\n"
				"x,y = amount to shift (in pixels - set these)\r\n"
				"w,h = width, height (in pixels)\r\n"
				"b = isBeat\r\n"
				"alpha = alpha value (0.0-1.0) for blend\r\n"
				;
			compilerfunctionlist( hwndDlg, text );
		}
		if( LOWORD( wParam ) == IDC_CHECK1 )
		{
			g_this->blend = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_CHECK2 )
		{
			g_this->subpixel = IsDlgButtonChecked( hwndDlg, IDC_CHECK2 ) ? 1 : 0;
		}
		if( !isstart && ( LOWORD( wParam ) == IDC_EDIT1 || LOWORD( wParam ) == IDC_EDIT2 || LOWORD( wParam ) == IDC_EDIT3 ) && HIWORD( wParam ) == EN_CHANGE )
		{
			CSLock __lock( renderLock );
			g_this->effect_exp[ 0 ].get_from_dlgitem( hwndDlg, IDC_EDIT1 );
			g_this->effect_exp[ 1 ].get_from_dlgitem( hwndDlg, IDC_EDIT2 );
			g_this->effect_exp[ 2 ].get_from_dlgitem( hwndDlg, IDC_EDIT3 );
		}
		return 0;
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_SHIFT ), hwndParent, g_DlgProc );
}