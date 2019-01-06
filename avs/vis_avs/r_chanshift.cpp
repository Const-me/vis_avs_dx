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
#include "resource.h"
#include "r_defs.h"

#ifndef LASER

// this will be the directory and APE name displayed in the AVS Editor
#define MOD_NAME "Trans / Channel Shift"

#define C_THISCLASS C_ChannelShiftClass


typedef struct {
	int	mode;
	int onbeat;
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
	int ids[] = { IDC_RBG, IDC_BRG, IDC_BGR, IDC_GBR, IDC_GRB, IDC_RGB };
	switch( uMsg )
	{
	case WM_COMMAND:
		if( HIWORD( wParam ) == BN_CLICKED ) {
			for( int i = 0; i < sizeof( ids ) / sizeof( ids[ 0 ] ); i++ )
				if( IsDlgButtonChecked( hwndDlg, ids[ i ] ) )
					g_ConfigThis->config.mode = ids[ i ];
			g_ConfigThis->config.onbeat = IsDlgButtonChecked( hwndDlg, IDC_ONBEAT ) ? 1 : 0;
		}
		return 1;


	case WM_INITDIALOG:
		g_ConfigThis->hwndDlg = hwndDlg;

		CheckDlgButton( hwndDlg, g_ConfigThis->config.mode, 1 );
		if( g_ConfigThis->config.onbeat )
			CheckDlgButton( hwndDlg, IDC_ONBEAT, 1 );

		return 1;

	case WM_DESTROY:
		KillTimer( hwndDlg, 1 );
		return 1;
	}
	return 0;
}

// set up default configuration 
C_THISCLASS::C_THISCLASS()
{
	memset( &config, 0, sizeof( apeconfig ) );
	config.mode = IDC_RBG;
	config.onbeat = 1;
	CREATE_DX_EFFECT( config.mode );
}

// virtual destructor
C_THISCLASS::~C_THISCLASS()
{
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_CHANSHIFT ), hwndParent, (DLGPROC)g_DlgProc );
}

char *C_THISCLASS::get_desc( void )
{
	return MOD_NAME;
}

void C_THISCLASS::load_config( unsigned char *data, int len )
{
	srand( time( 0 ) );
	if( len <= sizeof( apeconfig ) )
		memcpy( &this->config, data, len );
}


int  C_THISCLASS::save_config( unsigned char *data )
{
	memcpy( data, &this->config, sizeof( apeconfig ) );
	return sizeof( apeconfig );
}

C_RBASE *R_ChannelShift( char *desc )
{
	if( desc ) {
		strcpy( desc, MOD_NAME );
		return NULL;
	}
	return ( C_RBASE * ) new C_THISCLASS();
}


#endif