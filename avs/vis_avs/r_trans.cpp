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

#include "timing.h"
#include "avs_eelif.h"
#include <math.h>
#include <Threads/threadsApi.h>

#ifndef LASER

#define C_THISCLASS C_TransTabClass
#define MOD_NAME "Trans / Movement"

#define REFFECT_MIN 3
#define REFFECT_MAX 23

typedef struct {
	//    int index;       // Just here for descriptive purposes, makes it easy to match stuff up.
	char *list_desc; // The string to show in the listbox.
	char *eval_desc; // The optional string to display in the evaluation editor.
	char uses_eval;   // If this is true, the preset engages the eval library and there is NULL in the radial_effects array for its entry
	char uses_rect;   // This value sets the checkbox for rectangular calculation
} Description;

static Description descriptions[] =
{
  {/* 0,*/ "none", "", 0, 0},
  {/* 1,*/ "slight fuzzify", "", 0, 0},
  {/* 2,*/ "shift rotate left", "x=x+1.0/32; // use wrap for this one", 0, 1},
  {/* 3,*/ "big swirl out", "r = r + (0.1 - (0.2 * d));\r\nd = d * 0.96;", 0, 0},
  {/* 4,*/ "medium swirl", "d = d * (0.99 * (1.0 - sin(r-$PI*0.5) / 32.0));\r\nr = r + (0.03 * sin(d * $PI * 4));", 0, 0},
  {/* 5,*/ "sunburster", "d = d * (0.94 + (cos((r-$PI*0.5) * 32.0) * 0.06));", 0, 0},
  {/* 6,*/ "swirl to center", "d = d * (1.01 + (cos((r-$PI*0.5) * 4) * 0.04));\r\nr = r + (0.03 * sin(d * $PI * 4));", 0, 0},
  {/* 7,*/ "blocky partial out", "", 0, 0},
  {/* 8,*/ "swirling around both ways at once", "r = r + (0.1 * sin(d * $PI * 5));", 0, 0},
  {/* 9,*/ "bubbling outward", "t = sin(d * $PI);\r\nd = d - (8*t*t*t*t*t)/sqrt((sw*sw+sh*sh)/4);", 0, 0},
  {/*10,*/ "bubbling outward with swirl", "t = sin(d * $PI);\r\nd = d - (8*t*t*t*t*t)/sqrt((sw*sw+sh*sh)/4);\r\nt=cos(d*$PI/2.0);\r\nr= r + 0.1*t*t*t;", 0, 0},
  {/*11,*/ "5 pointed distro", "d = d * (0.95 + (cos(((r-$PI*0.5) * 5.0) - ($PI / 2.50)) * 0.03));", 0, 0},
  {/*12,*/ "tunneling", "r = r + 0.04;\r\nd = d * (0.96 + cos(d * $PI) * 0.05);", 0, 0},
  {/*13,*/ "bleedin'", "t = cos(d * $PI);\r\nr = r + (0.07 * t);\r\nd = d * (0.98 + t * 0.10);", 0, 0},
  {/*14,*/ "shifted big swirl out", "// this is a very bad approximation in script. fixme.\r\nd=sqrt(x*x+y*y); r=atan2(y,x);\r\nr=r+0.1-0.2*d; d=d*0.96;\r\nx=cos(r)*d + 8/128; y=sin(r)*d;", 0, 1},
  {/*15,*/ "psychotic beaming outward", "d = 0.15", 0, 0},
  {/*16,*/ "cosine radial 3-way", "r = cos(r * 3)", 0, 0},
  {/*17,*/ "spinny tube", "d = d * (1 - ((d - .35) * .5));\r\nr = r + .1;", 0, 0},
  {/*18,*/ "radial swirlies", "d = d * (1 - (sin((r-$PI*0.5) * 7) * .03));\r\nr = r + (cos(d * 12) * .03);", 1, 0},
  {/*19,*/ "swill", "d = d * (1 - (sin((r - $PI*0.5) * 12) * .05));\r\nr = r + (cos(d * 18) * .05);\r\nd = d * (1-((d - .4) * .03));\r\nr = r + ((d - .4) * .13)", 1, 0},
  {/*20,*/ "gridley", "x = x + (cos(y * 18) * .02);\r\ny = y + (sin(x * 14) * .03);", 1, 1},
  {/*21,*/ "grapevine", "x = x + (cos(abs(y-.5) * 8) * .02);\r\ny = y + (sin(abs(x-.5) * 8) * .05);\r\nx = x * .95;\r\ny = y * .95;", 1, 1},
  {/*22,*/ "quadrant", "y = y * ( 1 + (sin(r + $PI/2) * .3) );\r\nx = x * ( 1 + (cos(r + $PI/2) * .3) );\r\nx = x * .995;\r\ny = y * .995;", 1, 1},
  {/*23,*/ "6-way kaleida (use wrap!)", "y = (r*6)/($PI); x = d;", 1, 1},
};

static inline bool effect_uses_eval( int t )
{
	bool retval = false;
	if( ( t >= REFFECT_MIN ) && ( t <= REFFECT_MAX ) )
	{
		if( descriptions[ t ].uses_eval )
		{
			retval = true;
		}
	}
	return retval;
}


class C_THISCLASS : public C_RBASE2 {
protected:
public:
	C_THISCLASS();
	virtual ~C_THISCLASS();

	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );

	RString effect_exp;
	int effect_exp_ch;
	int effect, blend;
	int sourcemapped;
	int rectangular;
	int subpixel;
	int wrap;
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 ) { effect = GET_INT(); pos += 4; }
	if( effect == 32767 )
	{
		if( !memcmp( data + pos, "!rect ", 6 ) )
		{
			pos += 6;
			rectangular = 1;
		}
		if( data[ pos ] == 1 )
		{
			pos++;
			int l = GET_INT(); pos += 4;
			if( l > 0 && len - pos >= l )
			{
				effect_exp.resize( l );
				memcpy( effect_exp.get(), data + pos, l );
				pos += l;
			}
			else
			{
				effect_exp.resize( 1 );
				effect_exp.get()[ 0 ] = 0;
			}
		}
		else if( len - pos >= 256 )
		{
			char buf[ 257 ];
			int l = 256 - ( rectangular ? 6 : 0 );
			memcpy( buf, data + pos, l );
			buf[ l ] = 0;
			effect_exp.assign( buf );
			pos += l;
		}
	}
	if( len - pos >= 4 ) { blend = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { sourcemapped = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { rectangular = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { subpixel = GET_INT(); pos += 4; }
	else subpixel = 0;
	if( len - pos >= 4 ) { wrap = GET_INT(); pos += 4; }
	else wrap = 0;
	if( !effect && len - pos >= 4 )
	{
		effect = GET_INT(); pos += 4;
	}

	if( effect != 32767 && effect > REFFECT_MAX || effect < 0 )
		effect = 0;

	// Once we know what our _real_ effect value is, if it uses the evaluator, allocate and stuff the string, here.
	/*
	if (effect_uses_eval(effect))
	{
	  const char *eval_string = descriptions[effect].eval_desc;
	  int length = strlen(eval_string);
	  effect_exp.resize(length+1);
	  strcpy(effect_exp.get(), eval_string);
	}
	*/

	effect_exp_ch = 1;
}
int  C_THISCLASS::save_config( unsigned char *data )
{
	int pos = 0;

	// the reason this is 15, and not REFFECT_MAX, is because old versions of AVS barf
	// if effect is > 15, so we put 0, and at the end put the new value
	if( effect > 15 && effect != 32767 )
	{
		PUT_INT( 0 );
		pos += 4;
	}
	else
	{
		PUT_INT( effect );
		pos += 4;
	}
	if( effect == 32767 )
	{
		int l = strlen( effect_exp.get() ) + 1;
		data[ pos++ ] = 1;
		PUT_INT( l );
		pos += 4;
		memcpy( data + pos, effect_exp.get(), strlen( effect_exp.get() ) + 1 );
		pos += strlen( effect_exp.get() ) + 1;
	}
	PUT_INT( blend ); pos += 4;
	PUT_INT( sourcemapped ); pos += 4;
	PUT_INT( rectangular ); pos += 4;
	PUT_INT( subpixel ); pos += 4;
	PUT_INT( wrap ); pos += 4;

	// see note on '15' above =)
	if( effect > 15 && effect != 32767 )
	{
		PUT_INT( effect );
		pos += 4;
	}
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
	sourcemapped = 0;
	effect = 1;
	blend = 0;
	effect_exp.assign( "" );
	rectangular = 0;
	subpixel = 1;
	wrap = 0;
	effect_exp_ch = 1;

	CREATE_DX_EFFECT( effect_exp );
}

C_THISCLASS::~C_THISCLASS()
{ }

C_RBASE *R_Trans( char *desc )
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
	{
		int x;
		for( x = 0; x < sizeof( descriptions ) / sizeof( descriptions[ 0 ] ); x++ )
		{
			SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_ADDSTRING, 0, (long)( descriptions[ x ].list_desc ) );
		}

		isstart = 1;
		SetDlgItemText( hwndDlg, IDC_EDIT1, g_this->effect_exp.get() );
		// After we set whatever value into the edit box, that's the new saved value (ie: don't change the save format)
		isstart = 0;

		if( g_this->blend )
			CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
		if( g_this->subpixel )
			CheckDlgButton( hwndDlg, IDC_CHECK4, BST_CHECKED );
		if( g_this->wrap )
			CheckDlgButton( hwndDlg, IDC_WRAP, BST_CHECKED );
		if( g_this->rectangular )
			CheckDlgButton( hwndDlg, IDC_CHECK3, BST_CHECKED );
		if( g_this->sourcemapped & 2 )
			CheckDlgButton( hwndDlg, IDC_CHECK2, BST_INDETERMINATE );
		else if( g_this->sourcemapped & 1 )
			CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );

		SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_ADDSTRING, 0, ( long )"(user defined)" );
		SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_SETCURSEL, ( g_this->effect == 32767 ) ? sizeof( descriptions ) / sizeof( descriptions[ 0 ] ) : g_this->effect, 0 );
		if( g_this->effect == 32767 )
		{
			EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 1 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 1 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 1 );
		}
		else if( g_this->effect >= 0 && g_this->effect <= REFFECT_MAX )
		{
			if( strlen( descriptions[ g_this->effect ].eval_desc ) > 0 )
			{
				SetDlgItemText( hwndDlg, IDC_EDIT1, descriptions[ g_this->effect ].eval_desc );
				CheckDlgButton( hwndDlg, IDC_CHECK3, descriptions[ g_this->effect ].uses_rect ? BST_CHECKED : 0 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 1 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 1 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 1 );
			}
			else
			{
				EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 0 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 0 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 0 );
			}
		}
		else
		{
			EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 0 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 0 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 0 );
		}
	}
	return 1;
	case WM_TIMER:
		if( wParam == 1 )
		{
			KillTimer( hwndDlg, 1 );
			UPDATE_PARAMS();
			g_this->effect = 32767;
			g_this->effect_exp.get_from_dlgitem( hwndDlg, IDC_EDIT1 );
			g_this->effect_exp_ch = 1;
		}
		return 0;
	case WM_COMMAND:
		if( LOWORD( wParam ) == IDC_EDIT1 && HIWORD( wParam ) == EN_CHANGE )
		{
			KillTimer( hwndDlg, 1 );
			if( !isstart ) SetTimer( hwndDlg, 1, 1000, NULL );

			// If someone edits the editwnd, force the "(user defined)" to be the new selection.
			if( SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_GETCURSEL, 0, 0 ) != sizeof( descriptions ) / sizeof( descriptions[ 0 ] ) )
			{
				g_this->rectangular = IsDlgButtonChecked( hwndDlg, IDC_CHECK3 ) ? 1 : 0;
				SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_SETCURSEL, sizeof( descriptions ) / sizeof( descriptions[ 0 ] ), 0 );
			}
		}
		if( LOWORD( wParam ) == IDC_LIST1 && HIWORD( wParam ) == LBN_SELCHANGE )
		{
			int t;
			t = SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_GETCURSEL, 0, 0 );
			if( t == sizeof( descriptions ) / sizeof( descriptions[ 0 ] ) )
			{
				g_this->effect = 32767;
				EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 1 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 1 );
				EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 1 );

				SetDlgItemText( hwndDlg, IDC_EDIT1, g_this->effect_exp.get() );
				CheckDlgButton( hwndDlg, IDC_CHECK3, g_this->rectangular );

				// always reinit =)
				{
					UPDATE_PARAMS();
					g_this->effect_exp.get_from_dlgitem( hwndDlg, IDC_EDIT1 );
					g_this->effect_exp_ch = 1;
				}
			}
			else
			{
				g_this->effect = t;

				// If there is a string to stuff in the eval box,
				if( strlen( descriptions[ t ].eval_desc ) > 0 )
				{
					// save the value to be able to restore it later
					// stuff it and make sure the boxes are editable
					SetDlgItemText( hwndDlg, IDC_EDIT1, descriptions[ t ].eval_desc );
					EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 1 );
					EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 1 );
					EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 1 );
					CheckDlgButton( hwndDlg, IDC_CHECK3, descriptions[ t ].uses_rect ? BST_CHECKED : 0 );

					UPDATE_PARAMS();
					g_this->effect_exp.assign( descriptions[ t ].eval_desc );
					g_this->rectangular = descriptions[ t ].uses_rect;
				}
				else
				{
					// otherwise, they're not editable.
					CheckDlgButton( hwndDlg, IDC_CHECK3, g_this->rectangular ? BST_CHECKED : 0 );
					SetDlgItemText( hwndDlg, IDC_EDIT1, "" );
					EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), 0 );
					EnableWindow( GetDlgItem( hwndDlg, IDC_CHECK3 ), 0 );
					EnableWindow( GetDlgItem( hwndDlg, IDC_LABEL1 ), 0 );
				}
			}

		}
		if( LOWORD( wParam ) == IDC_CHECK1 )
		{
			g_this->blend = IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) ? 1 : 0;
		}
		if( LOWORD( wParam ) == IDC_CHECK4 )
		{
			g_this->subpixel = IsDlgButtonChecked( hwndDlg, IDC_CHECK4 ) ? 1 : 0;
			g_this->effect_exp_ch = 1;
		}
		if( LOWORD( wParam ) == IDC_WRAP )
		{
			g_this->wrap = IsDlgButtonChecked( hwndDlg, IDC_WRAP ) ? 1 : 0;
			g_this->effect_exp_ch = 1;
		}
		if( LOWORD( wParam ) == IDC_CHECK2 )
		{
			int a = IsDlgButtonChecked( hwndDlg, IDC_CHECK2 );
			if( a == BST_INDETERMINATE )
				g_this->sourcemapped = 2;
			else if( a == BST_CHECKED )
				g_this->sourcemapped = 1;
			else
				g_this->sourcemapped = 0;
		}
		if( LOWORD( wParam ) == IDC_CHECK3 )
		{
			g_this->rectangular = IsDlgButtonChecked( hwndDlg, IDC_CHECK3 ) ? 1 : 0;
			if( SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_GETCURSEL, 0, 0 ) != sizeof( descriptions ) / sizeof( descriptions[ 0 ] ) )
			{
				UPDATE_PARAMS();
				g_this->effect = 32767;
				g_this->effect_exp.get_from_dlgitem( hwndDlg, IDC_EDIT1 );
				g_this->effect_exp_ch = 1;
				SendDlgItemMessage( hwndDlg, IDC_LIST1, LB_SETCURSEL, sizeof( descriptions ) / sizeof( descriptions[ 0 ] ), 0 );
			}
			else
				g_this->effect_exp_ch = 1;
		}
		if( LOWORD( wParam ) == IDC_BUTTON2 )
		{
			char *text = "Movement\0"
				"Movement help goes here (send me some :)\r\n"
				"To use the custom table, modify r,d,x or y.\r\n"
				"Rect coords: x,y are in (-1..1) . Otherwise: d is (0..1) and r is (0..2PI).\r\n"
				"You can also access 'sw' and 'sh' for screen dimensions in pixels (might be useful)\r\n"
				;
			compilerfunctionlist( hwndDlg, text );
		}
		return 0;
		return 0;
	}
	return 0;
}


HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_TRANS ), hwndParent, g_DlgProc );
}

#else
C_RBASE *R_Trans( char *desc ) { return NULL; }
#endif