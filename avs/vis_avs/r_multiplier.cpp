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


#define MD_XI    0
#define MD_X8    1
#define MD_X4    2
#define MD_X2    3
#define MD_X05   4
#define MD_X025  5
#define MD_X0125 6
#define MD_XS    7

// this will be the directory and APE name displayed in the AVS Editor
#define MOD_NAME "Trans / Multiplier"
#define C_THISCLASS C_MultiplierClass

typedef struct {
	int	ml;
} apeconfig;

class C_THISCLASS : public C_RBASE
{
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual char *get_desc();
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	apeconfig config;

	HWND hwndDlg;
};

// global configuration dialog pointer 
static C_THISCLASS *g_ConfigThis;
static HINSTANCE g_hDllInstance;


// this is where we deal with the configuration screen
static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int id;
	switch( uMsg )
	{
	case WM_COMMAND:
		if( HIWORD( wParam ) == BN_CLICKED ) {
			id = LOWORD( wParam );
			switch( id ) {
			case IDC_XI:
				g_ConfigThis->config.ml = MD_XI;
				break;
			case IDC_X8:
				g_ConfigThis->config.ml = MD_X8;
				break;
			case IDC_X4:
				g_ConfigThis->config.ml = MD_X4;
				break;
			case IDC_X2:
				g_ConfigThis->config.ml = MD_X2;
				break;
			case IDC_X05:
				g_ConfigThis->config.ml = MD_X05;
				break;
			case IDC_X025:
				g_ConfigThis->config.ml = MD_X025;
				break;
			case IDC_X0125:
				g_ConfigThis->config.ml = MD_X0125;
				break;
			case IDC_XS:
				g_ConfigThis->config.ml = MD_XS;
				break;
			}
		}
		return 0;

	case WM_INITDIALOG:
		g_ConfigThis->hwndDlg = hwndDlg;

		switch( g_ConfigThis->config.ml ) {
		case MD_XI:
			id = IDC_XI;
			break;
		case MD_X8:
			id = IDC_X8;
			break;
		case MD_X4:
			id = IDC_X4;
			break;
		case MD_X2:
			id = IDC_X2;
			break;
		case MD_X05:
			id = IDC_X05;
			break;
		case MD_X025:
			id = IDC_X025;
			break;
		case MD_X0125:
			id = IDC_X0125;
			break;
		case MD_XS:
			id = IDC_XS;
			break;
		}
		SendMessage( GetDlgItem( hwndDlg, id ), BM_SETCHECK, BST_CHECKED, 0 );

		return 1;

	case WM_DESTROY:
		return 1;
	}
	return 0;
}

// set up default configuration 
C_THISCLASS::C_THISCLASS()
{
	memset( &config, 0, sizeof( apeconfig ) );
	config.ml = MD_X2;
	CREATE_DX_EFFECT( config.ml );
}

// virtual destructor
C_THISCLASS::~C_THISCLASS()
{ }

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_MULT ), hwndParent, (DLGPROC)g_DlgProc );
}

char *C_THISCLASS::get_desc( void )
{
	return MOD_NAME;
}

void C_THISCLASS::load_config( unsigned char *data, int len )
{
	if( len == sizeof( apeconfig ) )
		memcpy( &this->config, data, len );
	else
		memset( &this->config, 0, sizeof( apeconfig ) );
}

int  C_THISCLASS::save_config( unsigned char *data )
{
	memcpy( data, &this->config, sizeof( apeconfig ) );
	return sizeof( apeconfig );
}

C_RBASE *R_Multiplier( char *desc )
{
	if( desc ) {
		strcpy( desc, MOD_NAME );
		return NULL;
	}
	return ( C_RBASE * ) new C_THISCLASS();
}
#endif