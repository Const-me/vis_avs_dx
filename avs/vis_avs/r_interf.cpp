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

#define MOD_NAME "Trans / Interferences"
#define C_THISCLASS C_InterferencesClass

class C_THISCLASS : public C_RBASE {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	float GET_FLOAT( unsigned char *data, int pos );
	void PUT_FLOAT( float f, unsigned char *data, int pos );
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );
	int enabled;
	int nPoints;
	int distance;
	int alpha;
	int rotation;
	int rotationinc;
	int distance2;
	int alpha2;
	int rotationinc2;
	int rgb;
	int blend;
	int blendavg;
	float speed;
	int onbeat;
	float a;
	int _distance;
	int _alpha;
	int _rotationinc;
	int _rgb;
	float status;
};

#define PI 3.14159

static C_THISCLASS *g_ConfigThis; // global configuration dialog pointer 
static HINSTANCE g_hDllInstance; // global DLL instance pointer (not needed in this example, but could be useful)


C_THISCLASS::~C_THISCLASS()
{
}

// configuration read/write

C_THISCLASS::C_THISCLASS() // set up default configuration
{
	enabled = 1;
	nPoints = 2;
	distance = 10;
	alpha = 128;
	rotation = 0;
	rotationinc = 0;
	rgb = 1;
	blendavg = 0;
	blend = 0;
	onbeat = 1;
	distance2 = 32;
	rotationinc2 = 25;
	alpha2 = 192;
	status = 2;
	speed = ( float )0.2;
	a = (float)rotation / 255 * (float)PI * 2;

	CREATE_DX_EFFECT( enabled );
}

void C_THISCLASS::PUT_FLOAT( float f, unsigned char *data, int pos )
{
	int y = *(int *)&f;
	data[ pos ] = ( y ) & 255; data[ pos + 1 ] = ( y >> 8 ) & 255; data[ pos + 2 ] = ( y >> 16 ) & 255; data[ pos + 3 ] = ( y >> 24 ) & 255;
}

float C_THISCLASS::GET_FLOAT( unsigned char *data, int pos )
{
	int a = data[ pos ] | ( data[ pos + 1 ] << 8 ) | ( data[ pos + 2 ] << 16 ) | ( data[ pos + 3 ] << 24 );
	float f = *(float *)&a;
	return f;
}

#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len ) // read configuration of max length "len" from data.
{
	int pos = 0;
	if( len - pos >= 4 ) { enabled = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { nPoints = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rotation = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { distance = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { alpha = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rotationinc = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { blendavg = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { distance2 = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { alpha2 = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rotationinc2 = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rgb = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { onbeat = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { speed = GET_FLOAT( data, pos ); pos += 4; }
	a = (float)rotation / 255 * (float)PI * 2;
	status = (float)PI;
}

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
int  C_THISCLASS::save_config( unsigned char *data ) // write configuration to data, return length. config data should not exceed 64k.
{
	int pos = 0;

	PUT_INT( enabled ); pos += 4;
	PUT_INT( nPoints ); pos += 4;
	PUT_INT( rotation ); pos += 4;
	PUT_INT( distance ); pos += 4;
	PUT_INT( alpha ); pos += 4;
	PUT_INT( rotationinc ); pos += 4;
	PUT_INT( blend ); pos += 4;
	PUT_INT( blendavg ); pos += 4;
	PUT_INT( distance2 ); pos += 4;
	PUT_INT( alpha2 ); pos += 4;
	PUT_INT( rotationinc2 ); pos += 4;
	PUT_INT( rgb ); pos += 4;
	PUT_INT( onbeat ); pos += 4;
	PUT_FLOAT( speed, data, pos ); pos += 4;

	return pos;
}

// configuration dialog stuff
static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		SendDlgItemMessage( hwndDlg, IDC_NPOINTS, TBM_SETTICFREQ, 1, 0 );
		SendDlgItemMessage( hwndDlg, IDC_NPOINTS, TBM_SETRANGE, TRUE, MAKELONG( 0, 8 ) );
		SendDlgItemMessage( hwndDlg, IDC_NPOINTS, TBM_SETPOS, TRUE, g_ConfigThis->nPoints );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA, TBM_SETTICFREQ, 16, 0 );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA, TBM_SETRANGE, TRUE, MAKELONG( 1, 255 ) );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA, TBM_SETPOS, TRUE, g_ConfigThis->alpha );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE, TBM_SETTICFREQ, 8, 0 );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE, TBM_SETRANGE, TRUE, MAKELONG( 1, 64 ) );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE, TBM_SETPOS, TRUE, g_ConfigThis->distance );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE, TBM_SETTICFREQ, 2, 0 );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE, TBM_SETRANGE, TRUE, MAKELONG( 0, 64 ) );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE, TBM_SETPOS, TRUE, g_ConfigThis->rotationinc*-1 + 32 );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA2, TBM_SETTICFREQ, 16, 0 );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA2, TBM_SETRANGE, TRUE, MAKELONG( 1, 255 ) );
		SendDlgItemMessage( hwndDlg, IDC_ALPHA2, TBM_SETPOS, TRUE, g_ConfigThis->alpha2 );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE2, TBM_SETTICFREQ, 8, 0 );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE2, TBM_SETRANGE, TRUE, MAKELONG( 1, 64 ) );
		SendDlgItemMessage( hwndDlg, IDC_DISTANCE2, TBM_SETPOS, TRUE, g_ConfigThis->distance2 );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE2, TBM_SETTICFREQ, 2, 0 );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE2, TBM_SETRANGE, TRUE, MAKELONG( 0, 64 ) );
		SendDlgItemMessage( hwndDlg, IDC_ROTATE2, TBM_SETPOS, TRUE, g_ConfigThis->rotationinc2*-1 + 32 );
		SendDlgItemMessage( hwndDlg, IDC_INITROT, TBM_SETTICFREQ, 16, 0 );
		SendDlgItemMessage( hwndDlg, IDC_INITROT, TBM_SETRANGE, TRUE, MAKELONG( 0, 255 ) );
		SendDlgItemMessage( hwndDlg, IDC_INITROT, TBM_SETPOS, TRUE, 255 - g_ConfigThis->rotation );
		SendDlgItemMessage( hwndDlg, IDC_SPEED, TBM_SETTICFREQ, 1000, 0 );
		SendDlgItemMessage( hwndDlg, IDC_SPEED, TBM_SETRANGE, TRUE, MAKELONG( 1, 128 ) );
		SendDlgItemMessage( hwndDlg, IDC_SPEED, TBM_SETPOS, TRUE, (int)( g_ConfigThis->speed * 100 ) );
		if( g_ConfigThis->enabled ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_ConfigThis->blend ) CheckDlgButton( hwndDlg, IDC_ADDITIVE, BST_CHECKED );
		if( g_ConfigThis->blendavg ) CheckDlgButton( hwndDlg, IDC_5050, BST_CHECKED );
		if( !g_ConfigThis->blend && !g_ConfigThis->blendavg )
			CheckDlgButton( hwndDlg, IDC_REPLACE, BST_CHECKED );
		EnableWindow( GetDlgItem( hwndDlg, IDC_RGB ), ( g_ConfigThis->nPoints == 3 || g_ConfigThis->nPoints == 6 ) );
		CheckDlgButton( hwndDlg, IDC_RGB, g_ConfigThis->rgb ? BST_CHECKED : BST_UNCHECKED );
		CheckDlgButton( hwndDlg, IDC_ONBEAT, g_ConfigThis->onbeat ? BST_CHECKED : BST_UNCHECKED );
		return 1;
	case WM_COMMAND:
		if( ( LOWORD( wParam ) == IDC_CHECK1 ) ||
			( LOWORD( wParam ) == IDC_RGB ) ||
			( LOWORD( wParam ) == IDC_ONBEAT ) ||
			( LOWORD( wParam ) == IDC_ADDITIVE ) ||
			( LOWORD( wParam ) == IDC_REPLACE ) ||
			( LOWORD( wParam ) == IDC_5050 ) )
		{
			g_ConfigThis->enabled = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
			g_ConfigThis->blend = IsDlgButtonChecked( hwndDlg, IDC_ADDITIVE ) ? 1 : 0;
			g_ConfigThis->blendavg = IsDlgButtonChecked( hwndDlg, IDC_5050 ) ? 1 : 0;
			g_ConfigThis->rgb = IsDlgButtonChecked( hwndDlg, IDC_RGB ) ? 1 : 0;
			g_ConfigThis->onbeat = IsDlgButtonChecked( hwndDlg, IDC_ONBEAT ) ? 1 : 0;
		}
		return 0;
	case WM_NOTIFY:
		if( LOWORD( wParam ) == IDC_NPOINTS )
		{
			g_ConfigThis->nPoints = SendDlgItemMessage( hwndDlg, IDC_NPOINTS, TBM_GETPOS, 0, 0 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_RGB ), ( g_ConfigThis->nPoints == 3 || g_ConfigThis->nPoints == 6 ) );
		}
		if( LOWORD( wParam ) == IDC_ALPHA )
			g_ConfigThis->alpha = SendDlgItemMessage( hwndDlg, IDC_ALPHA, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_DISTANCE )
			g_ConfigThis->distance = SendDlgItemMessage( hwndDlg, IDC_DISTANCE, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_ROTATE )
			g_ConfigThis->rotationinc = -1 * ( SendDlgItemMessage( hwndDlg, IDC_ROTATE, TBM_GETPOS, 0, 0 ) - 32 );
		if( LOWORD( wParam ) == IDC_INITROT )
			g_ConfigThis->rotation = 255 - SendDlgItemMessage( hwndDlg, IDC_INITROT, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_SPEED )
			g_ConfigThis->speed = (float)SendDlgItemMessage( hwndDlg, IDC_SPEED, TBM_GETPOS, 0, 0 ) / 100;
		if( LOWORD( wParam ) == IDC_ALPHA2 )
			g_ConfigThis->alpha2 = SendDlgItemMessage( hwndDlg, IDC_ALPHA2, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_DISTANCE2 )
			g_ConfigThis->distance2 = SendDlgItemMessage( hwndDlg, IDC_DISTANCE2, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_ROTATE2 )
			g_ConfigThis->rotationinc2 = -1 * ( SendDlgItemMessage( hwndDlg, IDC_ROTATE2, TBM_GETPOS, 0, 0 ) - 32 );
		return 0;
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent ) // return NULL if no config dialog possible
{
	g_ConfigThis = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_INTERF ), hwndParent, g_DlgProc );
}

// export stuff

C_RBASE *R_Interferences( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}



#else
C_RBASE *R_Interferences( char *desc ) // creates a new effect object if desc is NULL, otherwise fills in desc with description
{
	return NULL;
}
#endif