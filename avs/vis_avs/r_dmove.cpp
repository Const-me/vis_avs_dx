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
#include "r_list.h"

#include "timing.h"

#if 0 
static void __docheck( int xp, int yp, int m_lastw, int m_lasth, int d_x, int d_y )
{
	xp >>= 16;
	yp >>= 16;
	if( xp < 0 || xp >= m_lastw || yp < 0 || yp >= m_lasth )
	{
		char buf[ 512 ];
		wsprintf( buf, "@ %d,%d on %d,%d (dx,dy=%d,%d)\n", xp, yp, m_lastw, m_lasth, d_x, d_y );
		OutputDebugString( buf );
	}
}
#endif


#ifndef LASER

#define C_THISCLASS C_DMoveClass
#define MOD_NAME "Trans / Dynamic Movement"

class C_THISCLASS : public C_RBASE2 {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();

	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	RString effect_exp[ 4 ];
	int m_xres, m_yres;
	int buffern;
	int subpixel, rectcoords, blend, wrap, nomove;
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
		char buf[ 1025 ];
		if( len - pos >= 1024 )
		{
			memcpy( buf, data + pos, 1024 );
			pos += 1024;
			buf[ 1024 ] = 0;
			effect_exp[ 3 ].assign( buf + 768 );
			buf[ 768 ] = 0;
			effect_exp[ 2 ].assign( buf + 512 );
			buf[ 512 ] = 0;
			effect_exp[ 1 ].assign( buf + 256 );
			buf[ 256 ] = 0;
			effect_exp[ 0 ].assign( buf );
		}
	}
	if( len - pos >= 4 ) { subpixel = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rectcoords = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { m_xres = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { m_yres = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { wrap = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { buffern = GET_INT(); pos += 4; }
	else buffern = 0;
	if( len - pos >= 4 ) { nomove = GET_INT(); pos += 4; }
	else nomove = 0;

}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;
	data[ pos++ ] = 1;
	save_string( data, pos, effect_exp[ 0 ] );
	save_string( data, pos, effect_exp[ 1 ] );
	save_string( data, pos, effect_exp[ 2 ] );
	save_string( data, pos, effect_exp[ 3 ] );
	PUT_INT( subpixel ); pos += 4;
	PUT_INT( rectcoords ); pos += 4;
	PUT_INT( m_xres ); pos += 4;
	PUT_INT( m_yres ); pos += 4;
	PUT_INT( blend ); pos += 4;
	PUT_INT( wrap ); pos += 4;
	PUT_INT( buffern ); pos += 4;
	PUT_INT( nomove ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	effect_exp[ 0 ].assign( "" );
	effect_exp[ 1 ].assign( "" );
	effect_exp[ 2 ].assign( "" );
	effect_exp[ 3 ].assign( "" );

	m_xres = 16;
	m_yres = 16;
	subpixel = 1;
	rectcoords = 0;
	blend = 0;
	wrap = 0;
	buffern = 0;
	nomove = 0;

	CREATE_DX_EFFECT( effect_exp );
}

C_THISCLASS::~C_THISCLASS()
{ }

C_RBASE *R_DMove( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}

typedef struct
{
	char *name;
	int rect;
	int wrap;
	int grid1;
	int grid2;
	char *init;
	char *point;
	char *frame;
	char *beat;
} presetType;

static presetType presets[] =
{
	// {"Random Rotate", 0, 1, 2, 2, "","r = r + dr;","","dr = (rand(100) / 100) * $PI;\r\nd = d * .95;"},
	{"Random Rotate", 0, 1, 2, 2, "","r = r + dr;\r\nd = if( b, d * 0.95, d );","","dr = (rand(100) / 100) * $PI"},
	{"Random Direction", 1, 1, 2, 2, "speed=.05;dr = (rand(200) / 100) * $PI;","x = x + dx;\r\ny = y + dy;","dx = cos(dr) * speed;\r\ndy = sin(dr) * speed;","dr = (rand(200) / 100) * $PI;"},
	{"In and Out", 0, 1, 2, 2, "speed=.2;c=0;","d = d * dd;","","c = c + ($PI/2);\r\ndd = 1 - (sin(c) * speed);"},
	{"Unspun Kaleida", 0, 1, 33, 33, "c=200;f=0;dt=0;dl=0;beatdiv=8","r=cos(r*dr);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 4+(cos(dt)*2);","c=f;f=0;dl=dt"},
	{"Roiling Gridley", 1, 1, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","x=x+(sin(y*dx) * .03);\r\ny=y-(cos(x*dy) * .03);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndx = 14+(cos(dt)*8);\r\ndy = 10+(sin(dt*2)*4);","c=f;f=0;dl=dt"},
	{"6-Way Outswirl", 0, 0, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","d=d*(1+(cos(r*6) * .05));\r\nr=r-(sin(d*dr) * .05);\r\nd = d * .98;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 18+(cos(dt)*12);","c=f;f=0;dl=dt"},
	{"Wavy", 1, 1, 6, 6, "c=200;f=0;dx=0;dl=0;beatdiv=16;speed=.05","y = y + ((sin((x+dx) * $PI))*speed);\r\nx = x + .025","f = f + 1;\r\nt = ( (f * 2 * 3.1415) / c ) / beatdiv;\r\ndx = dl + t;","c = f;\r\nf = 0;\r\ndl = dx;"},
	{"Smooth Rotoblitter", 0, 1, 2, 2, "c=200;f=0;dt=0;dl=0;beatdiv=4;speed=.15","r = r + dr;\r\nd = d * dd;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = cos(dt)*speed*2;\r\ndd = 1 - (sin(dt)*speed);","c=f;f=0;dl=dt"},
};


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
		SetDlgItemText( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 3 ].get() );
		if( g_this->blend )
			CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_this->subpixel )
			CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );
		if( g_this->rectcoords )
			CheckDlgButton( hwndDlg, IDC_CHECK3, BST_CHECKED );
		if( g_this->wrap )
			CheckDlgButton( hwndDlg, IDC_WRAP, BST_CHECKED );
		if( g_this->nomove )
			CheckDlgButton( hwndDlg, IDC_NOMOVEMENT, BST_CHECKED );

		SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_ADDSTRING, 0, ( int )"Current" );
		{
			int i = 0;
			char txt[ 64 ];
			for( i = 0; i < NBUF; i++ )
			{
				wsprintf( txt, "Buffer %d", i + 1 );
				SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_ADDSTRING, 0, (int)txt );
			}
		}
		SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_SETCURSEL, (WPARAM)g_this->buffern, 0 );

		SetDlgItemInt( hwndDlg, IDC_EDIT5, g_this->m_xres, FALSE );
		SetDlgItemInt( hwndDlg, IDC_EDIT6, g_this->m_yres, FALSE );
		isstart = 0;

		return 1;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_BUTTON1 )
		{
			char *text = "Dynamic Movement\0"
				"Dynamic movement help goes here (send me some :)";
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
		if( LOWORD( wParam ) == IDC_CHECK3 )
		{
			g_this->rectcoords = IsDlgButtonChecked( hwndDlg, IDC_CHECK3 ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_WRAP )
		{
			g_this->wrap = IsDlgButtonChecked( hwndDlg, IDC_WRAP ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_NOMOVEMENT )
		{
			g_this->nomove = IsDlgButtonChecked( hwndDlg, IDC_NOMOVEMENT ) ? 1 : 0;
		}
		// Load preset examples from the examples table.
		if( LOWORD( wParam ) == IDC_BUTTON4 )
		{
			RECT r;
			HMENU hMenu;
			MENUITEMINFO i = { sizeof( i ), };
			hMenu = CreatePopupMenu();
			int x;
			for( x = 0; x < sizeof( presets ) / sizeof( presets[ 0 ] ); x++ )
			{
				i.fMask = MIIM_TYPE | MIIM_DATA | MIIM_ID;
				i.fType = MFT_STRING;
				i.wID = x + 16;
				i.dwTypeData = presets[ x ].name;
				i.cch = strlen( presets[ x ].name );
				InsertMenuItem( hMenu, x, TRUE, &i );
			}
			GetWindowRect( GetDlgItem( hwndDlg, IDC_BUTTON1 ), &r );
			x = TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTBUTTON | TPM_NONOTIFY, r.right, r.top, 0, hwndDlg, NULL );
			if( x >= 16 && x < 16 + sizeof( presets ) / sizeof( presets[ 0 ] ) )
			{
				SetDlgItemText( hwndDlg, IDC_EDIT1, presets[ x - 16 ].point );
				SetDlgItemText( hwndDlg, IDC_EDIT2, presets[ x - 16 ].frame );
				SetDlgItemText( hwndDlg, IDC_EDIT3, presets[ x - 16 ].beat );
				SetDlgItemText( hwndDlg, IDC_EDIT4, presets[ x - 16 ].init );
				SetDlgItemInt( hwndDlg, IDC_EDIT5, presets[ x - 16 ].grid1, FALSE );
				SetDlgItemInt( hwndDlg, IDC_EDIT6, presets[ x - 16 ].grid2, FALSE );
				if( presets[ x - 16 ].rect )
				{
					g_this->rectcoords = 1;
					CheckDlgButton( hwndDlg, IDC_CHECK3, BST_CHECKED );
				}
				else
				{
					g_this->rectcoords = 0;
					CheckDlgButton( hwndDlg, IDC_CHECK3, 0 );
				}
				if( presets[ x - 16 ].wrap )
				{
					g_this->wrap = 1;
					CheckDlgButton( hwndDlg, IDC_WRAP, BST_CHECKED );
				}
				else
				{
					g_this->wrap = 0;
					CheckDlgButton( hwndDlg, IDC_WRAP, 0 );
				}
				SendMessage( hwndDlg, WM_COMMAND, MAKEWPARAM( IDC_EDIT4, EN_CHANGE ), 0 );
			}
			DestroyMenu( hMenu );
		}

		if( !isstart && HIWORD( wParam ) == CBN_SELCHANGE && LOWORD( wParam ) == IDC_COMBO1 ) // handle clicks to combo box
			g_this->buffern = SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0 );

		if( !isstart && HIWORD( wParam ) == EN_CHANGE )
		{
			if( LOWORD( wParam ) == IDC_EDIT5 || LOWORD( wParam ) == IDC_EDIT6 )
			{
				BOOL t;
				g_this->m_xres = GetDlgItemInt( hwndDlg, IDC_EDIT5, &t, 0 );
				g_this->m_yres = GetDlgItemInt( hwndDlg, IDC_EDIT6, &t, 0 );
			}

			if( LOWORD( wParam ) == IDC_EDIT1 || LOWORD( wParam ) == IDC_EDIT2 || LOWORD( wParam ) == IDC_EDIT3 || LOWORD( wParam ) == IDC_EDIT4 )
			{
				UPDATE_PARAMS();
				g_this->effect_exp[ 0 ].get_from_dlgitem( hwndDlg, IDC_EDIT1 );
				g_this->effect_exp[ 1 ].get_from_dlgitem( hwndDlg, IDC_EDIT2 );
				g_this->effect_exp[ 2 ].get_from_dlgitem( hwndDlg, IDC_EDIT3 );
				g_this->effect_exp[ 3 ].get_from_dlgitem( hwndDlg, IDC_EDIT4 );
			}
		}
		return 0;
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_DMOVE ), hwndParent, g_DlgProc );
}

#else
C_RBASE *R_DMove( char *desc ) { return NULL; }
#endif