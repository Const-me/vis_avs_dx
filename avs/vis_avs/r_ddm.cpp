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
#define M_PI 3.14159265358979323846

#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include "r_defs.h"
#include "resource.h"
#include "avs_eelif.h"

#include "timing.h"
#include <atlbase.h>
using CSLock = CComCritSecLock<CComAutoCriticalSection>;

#ifndef LASER

#define C_THISCLASS C_PulseClass
#define MOD_NAME "Trans / Dynamic Distance Modifier"

class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );
	RString effect_exp[ 4 ];
	int blend;
	int subpixel;
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
		load_string( effect_exp[ 3 ], data, pos, len );
	}
	else
	{
		char buf[ 513 ];
		if( len - pos >= 256 * 2 )
		{
			memcpy( buf, data + pos, 256 * 2 );
			pos += 256 * 2;
			buf[ 512 ] = 0;
			effect_exp[ 1 ].assign( buf + 256 );
			buf[ 256 ] = 0;
			effect_exp[ 0 ].assign( buf );
		}
		if( len - pos >= 256 * 2 )
		{
			memcpy( buf, data + pos, 256 * 2 );
			pos += 256 * 2;
			buf[ 512 ] = 0;
			effect_exp[ 3 ].assign( buf + 256 );
			buf[ 256 ] = 0;
			effect_exp[ 2 ].assign( buf );
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
	save_string( data, pos, effect_exp[ 3 ] );
	PUT_INT( blend ); pos += 4;
	PUT_INT( subpixel ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	effect_exp[ 0 ].assign( "d=d-sigmoid((t-50)/100,2)" );
	effect_exp[ 3 ].assign( "u=1;t=0" );
	effect_exp[ 1 ].assign( "t=t+u;t=min(100,t);t=max(0,t);u=if(equal(t,100),-1,u);u=if(equal(t,0),1,u)" );
	effect_exp[ 2 ].assign( "" );
	blend = 0;
	subpixel = 0;

	CREATE_DX_EFFECT( effect_exp[ 0 ] );
}

C_THISCLASS::~C_THISCLASS()
{ }

C_RBASE *R_DDM( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

static C_THISCLASS *g_this;
extern ATL::CComAutoCriticalSection renderLock;

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
		SetDlgItemText( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 3 ].get() );
		isstart = 0;
		if( g_this->blend )
			CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_this->subpixel )
			CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );
		return 1;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_BUTTON1 )
		{
			char *text = "Dynamic Distance Modifier\0"
				"The dynamic distance modifier allows you to dynamically (once per frame)\r\n"
				"change the source pixels for each ring of pixels out from the center.\r\n"
				"In the 'pixel' code section, 'd' represents the distance in pixels\r\n"
				"the current ring is from the center, and code can modify it to\r\n"
				"change the distance from the center where the source pixels for\r\n"
				"that ring would be read. This is a terrible explanation, and if\r\n"
				"you want to make a better one send it to me. \r\n"
				"\r\n"
				"Examples:\r\n"
				"Zoom in: 'd=d*0.9'\r\n"
				"Zoom out: 'd=d*1.1'\r\n"
				"Back and forth: pixel='d=d*(1.0+0.1*cos(t));', frame='t=t+0.1'\r\n"
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
		if( !isstart && ( LOWORD( wParam ) == IDC_EDIT1 || LOWORD( wParam ) == IDC_EDIT2 || LOWORD( wParam ) == IDC_EDIT3 || LOWORD( wParam ) == IDC_EDIT4 ) && HIWORD( wParam ) == EN_CHANGE )
		{
			CSLock __lock{ renderLock };
			g_this->effect_exp[ 0 ].get_from_dlgitem( hwndDlg, IDC_EDIT1 );
			g_this->effect_exp[ 1 ].get_from_dlgitem( hwndDlg, IDC_EDIT2 );
			g_this->effect_exp[ 2 ].get_from_dlgitem( hwndDlg, IDC_EDIT3 );
			g_this->effect_exp[ 3 ].get_from_dlgitem( hwndDlg, IDC_EDIT4 );
		}
		return 0;
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_DDM ), hwndParent, g_DlgProc );
}
#else
C_RBASE *R_DDM( char *desc ) { return NULL; }
#endif