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
#if 0//syntax highlighting
#include "richedit.h"
#endif
#include "timing.h"

#define C_THISCLASS C_SScopeClass
#define MOD_NAME "Render / SuperScope"



class C_THISCLASS : public C_RBASE {
protected:
	static BOOL CALLBACK g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	C_THISCLASS();
	virtual ~C_THISCLASS();
	virtual char *get_desc() { return MOD_NAME; }
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent );
	virtual void load_config( unsigned char *data, int len );
	virtual int  save_config( unsigned char *data );
	RString effect_exp[ 4 ];
	int which_ch;
	int num_colors;
	int colors[ 16 ];
	int mode;
};

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))
void C_THISCLASS::load_config( unsigned char *data, int len )
{
	int pos = 0;
	int x = 0;
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
	if( len - pos >= 4 ) { which_ch = GET_INT(); pos += 4; }
	if( len - pos >= 4 ) { num_colors = GET_INT(); pos += 4; }
	if( num_colors <= 16 ) while( len - pos >= 4 && x < num_colors ) { colors[ x++ ] = GET_INT(); pos += 4; }
	else num_colors = 0;
	if( len - pos >= 4 ) { mode = GET_INT(); pos += 4; }
}

int  C_THISCLASS::save_config( unsigned char *data )
{
	int x = 0;
	int pos = 0;
	data[ pos++ ] = 1;
	save_string( data, pos, effect_exp[ 0 ] );
	save_string( data, pos, effect_exp[ 1 ] );
	save_string( data, pos, effect_exp[ 2 ] );
	save_string( data, pos, effect_exp[ 3 ] );
	PUT_INT( which_ch ); pos += 4;
	PUT_INT( num_colors ); pos += 4;
	while( x < num_colors ) { PUT_INT( colors[ x ] ); x++;  pos += 4; }
	PUT_INT( mode ); pos += 4;
	return pos;
}

C_THISCLASS::C_THISCLASS()
{
#ifdef LASER
	mode = 1;
#else
	mode = 0;
#endif

	which_ch = 2;
	num_colors = 1;
	memset( colors, 0, sizeof( colors ) );
	colors[ 0 ] = RGB( 255, 255, 255 );

	effect_exp[ 0 ].assign( "d=i+v*0.2; r=t+i*$PI*4; x=cos(r)*d; y=sin(r)*d" );
	effect_exp[ 1 ].assign( "t=t-0.05" );
	effect_exp[ 2 ].assign( "" );
	effect_exp[ 3 ].assign( "n=800" );

	CREATE_DX_EFFECT( effect_exp );
}

C_THISCLASS::~C_THISCLASS()
{ }

static __inline int makeint( double t )
{
	if( t <= 0.0 ) return 0;
	if( t >= 1.0 ) return 255;
	return (int)( t*255.0 );
}

C_RBASE *R_SScope( char *desc )
{
	if( desc ) { strcpy( desc, MOD_NAME ); return NULL; }
	return ( C_RBASE * ) new C_THISCLASS();
}
typedef struct
{
	char *name;
	char *init;
	char *point;
	char *frame;
	char *beat;
} presetType;

static presetType presets[] =
{
#ifdef LASER
  {"Laser - Bouncing Line","n=8;xp=0;yp=0;d=0.5;r=0;xps=0; yps=0; dtr=0.1; bv=1; gv=1; rv=1;","x=xp+d*cos(r)*sign(i-0.5); y=yp+d*sin(r)*sign(i-0.5);","r=r+dtr; xp=xp*0.99+xps*0.01; yp=yp*0.99+yps*0.01; red=rv; green=gv; blue=bv;","xps=(rand(100)-50)/50; yps=(rand(100)-50)/50; dtr=-dtr; rv=rand(100)/100; gv=rand(100)/100; bv=rand(100)/100;"},
  {"Laser - BeatFlex Scope","n=5; tv=0; dtv=0; tvs=1.0;","x=(i-0.5)*2; y=-sin(i*$PI)*tv+v*0.2;","tv=tv*0.97+tvs*0.03;","tvs=-sign(tvs);"},
  {"Laser - Pulsing Box","n=5;r=$PI/4;","x=cos(i*$PI*2+r); y=sin(i*$PI*2+r); red=rv; green=gv; blue=bv;","rv=rv*0.93; gv=gv*0.93; bv=bv*0.93;","bv=blue; gv=green;rv=red;"},
#else
  {"Spiral","n=800","d=i+v*0.2; r=t+i*$PI*4; x=cos(r)*d; y=sin(r)*d","t=t-0.05",""},
  {"3D Scope Dish", "n=200","iz=1.3+sin(r+i*$PI*2)*(v+0.5)*0.88; ix=cos(r+i*$PI*2)*(v+0.5)*.88; iy=-0.3+abs(cos(v*$PI)); x=ix/iz;y=iy/iz;","",""},
  {"Rotating Bow Thing","n=80;t=0.0;","r=i*$PI*2; d=sin(r*3)+v*0.5; x=cos(t+r)*d; y=sin(t-r)*d","t=t+0.01",""},
  {"Vertical Bouncing Scope","n=100; t=0; tv=0.1;dt=1;","x=t+v*pow(sin(i*$PI),2); y=i*2-1.0;","t=t*0.9+tv*0.1","tv=((rand(50.0)/50.0))*dt; dt=-dt;"},
  {"Spiral Graph Fun","n=100;t=0;","r=i*$PI*128+t; x=cos(r/64)*0.7+sin(r)*0.3; y=sin(r/64)*0.7+cos(r)*0.3","t=t+0.01;","n=80+rand(120.0)"},
  {"Alternating Diagonal Scope","n=64; t=1;","sc=0.4*sin(i*$PI); x=2*(i-0.5-v*sc)*t; y=2*(i-0.5+v*sc);","","t=-t;"},
  {"Vibrating Worm","n=w; dt=0.01; t=0; sc=1;","x=cos(2*i+t)*0.9*(v*0.5+0.5); y=sin(i*2+t)*0.9*(v*0.5+0.5);","t=t+dt;dt=0.9*dt+0.001; t=if(above(t,$PI*2),t-$PI*2,t);","dt=sc;sc=-sc;"},
  {"Wandering Simple","n=800;xa=-0.5;ya=0.0;xb=-0.0;yb=0.75;c=200;f=0;\r\nnxa=(rand(100)-50)*.02;nya=(rand(100)-50)*.02;\r\nnxb=(rand(100)-50)*.02;nyb=(rand(100)-50)*.02;","//primary render\r\nx=(ex*i)+xa;\r\ny=(ey*i)+ya;\r\n\r\n//volume offset\r\nx=x+ ( cos(r) * v * dv);\r\ny=y+ ( sin(r) * v * dv);\r\n\r\n//color values\r\nred=i;\r\ngreen=(1-i);\r\nblue=abs(v*6);","f=f+1;\r\nt=1-((cos((f*3.1415)/c)+1)*.5);\r\nxa=((nxa-lxa)*t)+lxa;\r\nya=((nya-lya)*t)+lya;\r\nxb=((nxb-lxb)*t)+lxb;\r\nyb=((nyb-lyb)*t)+lyb;\r\nex=(xb-xa);\r\ney=(yb-ya);\r\nd=sqrt(sqr(ex)+sqr(ey));\r\nr=atan(ey/ex)+(3.1415/2);\r\ndv=d*2","c=f;\r\nf=0;\r\nlxa=nxa;\r\nlya=nya;\r\nlxb=nxb;\r\nlyb=nyb;\r\nnxa=(rand(100)-50)*.02;\r\nnya=(rand(100)-50)*.02;\r\nnxb=(rand(100)-50)*.02;\r\nnyb=(rand(100)-50)*.02"},
  {"Flitterbug","n=180;t=0.0;lx=0;ly=0;vx=rand(200)-100;vy=rand(200)-100;cf=.97;c=200;f=0","x=nx;y=ny;\r\nr=i*3.14159*2; d=(sin(r*5*(1-s))+i*0.5)*(.3-s);\r\ntx=(t*(1-(s*(i-.5))));\r\nx=x+cos(tx+r)*d; y=y+sin(t-y)*d;\r\nred=abs(x-nx)*5;\r\ngreen=abs(y-ny)*5;\r\nblue=1-s-red-green;","f=f+1;t=(f*2*3.1415)/c;\r\nvx=(vx-(lx*.1))*cf;\r\nvy=(vy-(ly*.1))*cf;\r\nlx=lx+vx;ly=ly+vy;\r\nnx=lx*.001;ny=ly*.001;\r\ns=abs(nx*ny)","c=f;f=0;\r\nvx=vx+rand(600)-300;vy=vy+rand(600)-300"},
  {"Spirostar","n=20;t=0;f=0;c=200;mn=10;dv=2;dn=0","r=if(b,0,((i*dv)*3.14159*128)+(t/2));\r\nx=cos(r)*sz;\r\ny=sin(r)*sz;","f=f+1;t=(f*3.1415*2)/c;\r\nsz=abs(sin(t-3.1415));\r\ndv=if(below(n,12),(n/2)-1,\r\n    if(equal(12,n),3,\r\n    if(equal(14,n),6,\r\n    if(below(n,20),2,4))))","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);\r\ng=if(equal(bb%beatdiv,0),g+1,g);\r\nn=if(equal(bb%beatdiv,0),(abs((g%17)-8) *2)+4,n);"},
  {"Exploding Daisy","n = 380 + rand(200) ; k = 0.0; l = 0.0; m = ( rand( 10 ) + 2 ) * .5; c = 0; f = 0","r=(i*3.14159*2)+(a * 3.1415);\r\nd=sin(r*m)*.3;\r\nx=cos(k+r)*d*2;y=(  (sin(k-r)*d) + ( sin(l*(i-.5) ) ) ) * .7;\r\nred=abs(x);\r\ngreen=abs(y);\r\nblue=d","a = a + 0.002 ; k = k + 0.04 ; l = l + 0.03","bb = bb + 1;\r\nbeatdiv = 16;\r\nn=if(equal(bb%beatdiv,0),380 + rand(200),n);\r\nt=if(equal(bb%beatdiv,0),0.0,t);\r\na=if(equal(bb%beatdiv,0),0.0,a);\r\nk=if(equal(bb%beatdiv,0),0.0,k);\r\nl=if(equal(bb%beatdiv,0),0.0,l);\r\nm=if(equal(bb%beatdiv,0),(( rand( 100  ) + 2 ) * .1) + 2,m);"},
  {"Swirlie Dots","n=45;t=rand(100);u=rand(100)","di = ( i - .5) * 2;\r\nx = di;sin(u*di) * .4;\r\ny = cos(u*di) * .6;\r\nx = x + ( cos(t) * .05 );\r\ny = y + ( sin(t) * .05 );","t = t + .15; u = u + .05","bb = bb + 1;\r\nbeatdiv = 16;\r\nn = if(equal(bb%beatdiv,0),30 + rand( 30 ),n);"},
  {"Sweep","n=180;lsv=100;sv=200;ssv=200;c=200;f=0","sv=(sv*abs(cos(lsv)))+(lsv*abs(cos(sv)));\r\nfv=fv+(sin(sv)*dv);\r\nd=i; r=t+(fv * sin(t) * .3)*3.14159*4;\r\nx=cos(r)*d;\r\ny=sin(r)*d;\r\nred=i;\r\ngreen=abs(sin(r))-(red*.15);\r\nblue=fv","f=f+1;t=(f*2*3.1415)/c;\r\nlsv=slsv;sv=ssv;fv=0","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);\r\ndv=if(equal(bb%beatdiv,0),((rand(100)*.01) * .1) + .02,dv);\r\nn=if(equal(bb%beatdiv,0),80+rand(100),n);\r\nssv=if(equal(bb%beatdiv,0),rand(200)+100,ssv);\r\nslsv=if(equal(bb%beatdiv,0),rand(200)+100,slsv);"},
  {"Whiplash Spiral","n=80;c=200;f=0","d=i;\r\nr=t+i*3.14159*4;\r\nsdt=sin(dt+(i*3.1415*2));\r\ncdt=cos(dt+(i*3.1415*2));\r\nx=(cos(r)*d) + (sdt * .6 * sin(t) );\r\ny=(sin(r)*d) + ( cdt *.6 * sin(t) );\r\nblue=abs(x);\r\ngreen=abs(y);\r\nred=cos(dt*4)","t=t-0.05;f=f+1;dt=(f*2*3.1415)/c","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);"},
#endif
};

static C_THISCLASS *g_this;
BOOL CALLBACK C_THISCLASS::g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int isstart;
	switch( uMsg )
	{
	case WM_INITDIALOG:
		isstart = 1;
#if 0//syntax highlighting
		SendDlgItemMessage( hwndDlg, IDC_EDIT1, EM_SETEVENTMASK, 0, ENM_CHANGE );
		SendDlgItemMessage( hwndDlg, IDC_EDIT2, EM_SETEVENTMASK, 0, ENM_CHANGE );
		SendDlgItemMessage( hwndDlg, IDC_EDIT3, EM_SETEVENTMASK, 0, ENM_CHANGE );
		SendDlgItemMessage( hwndDlg, IDC_EDIT4, EM_SETEVENTMASK, 0, ENM_CHANGE );
#endif
		SetDlgItemInt( hwndDlg, IDC_NUMCOL, g_this->num_colors, FALSE );
		SetDlgItemText( hwndDlg, IDC_EDIT1, g_this->effect_exp[ 0 ].get() );
		SetDlgItemText( hwndDlg, IDC_EDIT2, g_this->effect_exp[ 1 ].get() );
		SetDlgItemText( hwndDlg, IDC_EDIT3, g_this->effect_exp[ 2 ].get() );
		SetDlgItemText( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 3 ].get() );

#if 0//syntax highlighting
		doAVSEvalHighLight( hwndDlg, IDC_EDIT1, g_this->effect_exp[ 0 ].get() );
		doAVSEvalHighLight( hwndDlg, IDC_EDIT2, g_this->effect_exp[ 1 ].get() );
		doAVSEvalHighLight( hwndDlg, IDC_EDIT3, g_this->effect_exp[ 2 ].get() );
		doAVSEvalHighLight( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 3 ].get() );
#endif

		CheckDlgButton( hwndDlg, g_this->mode ? IDC_LINES : IDC_DOT, BST_CHECKED );
		if( ( g_this->which_ch & 3 ) == 0 )
			CheckDlgButton( hwndDlg, IDC_LEFTCH, BST_CHECKED );
		else if( ( g_this->which_ch & 3 ) == 1 )
			CheckDlgButton( hwndDlg, IDC_RIGHTCH, BST_CHECKED );
		else
			CheckDlgButton( hwndDlg, IDC_MIDCH, BST_CHECKED );
		if( g_this->which_ch & 4 )
			CheckDlgButton( hwndDlg, IDC_SPEC, BST_CHECKED );
		else
			CheckDlgButton( hwndDlg, IDC_WAVE, BST_CHECKED );

		isstart = 0;
		return 1;
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *di = (DRAWITEMSTRUCT *)lParam;
		if( di->CtlID == IDC_DEFCOL && g_this->num_colors > 0 )
		{
			int x;
			int w = di->rcItem.right - di->rcItem.left;
			int l = 0, nl;
			for( x = 0; x < g_this->num_colors; x++ )
			{
				int color = g_this->colors[ x ];
				nl = ( w*( x + 1 ) ) / g_this->num_colors;
				color = ( ( color >> 16 ) & 0xff ) | ( color & 0xff00 ) | ( ( color << 16 ) & 0xff0000 );

				HPEN hPen, hOldPen;
				HBRUSH hBrush, hOldBrush;
				LOGBRUSH lb = { BS_SOLID,color,0 };
				hPen = (HPEN)CreatePen( PS_SOLID, 0, color );
				hBrush = CreateBrushIndirect( &lb );
				hOldPen = (HPEN)SelectObject( di->hDC, hPen );
				hOldBrush = (HBRUSH)SelectObject( di->hDC, hBrush );
				Rectangle( di->hDC, di->rcItem.left + l, di->rcItem.top, di->rcItem.left + nl, di->rcItem.bottom );
				SelectObject( di->hDC, hOldPen );
				SelectObject( di->hDC, hOldBrush );
				DeleteObject( hBrush );
				DeleteObject( hPen );
				l = nl;
			}
		}
	}
	return 0;
	case WM_COMMAND:
		if( !isstart )
		{
			if( ( LOWORD( wParam ) == IDC_EDIT1 || LOWORD( wParam ) == IDC_EDIT2 || LOWORD( wParam ) == IDC_EDIT3 || LOWORD( wParam ) == IDC_EDIT4 ) && HIWORD( wParam ) == EN_CHANGE )
			{
				UPDATE_PARAMS();
				g_this->effect_exp[ 0 ].get_from_dlgitem( hwndDlg, IDC_EDIT1 );
				g_this->effect_exp[ 1 ].get_from_dlgitem( hwndDlg, IDC_EDIT2 );
				g_this->effect_exp[ 2 ].get_from_dlgitem( hwndDlg, IDC_EDIT3 );
				g_this->effect_exp[ 3 ].get_from_dlgitem( hwndDlg, IDC_EDIT4 );
#if 0//syntax highlighting
				if( LOWORD( wParam ) == IDC_EDIT1 )
					doAVSEvalHighLight( hwndDlg, IDC_EDIT1, g_this->effect_exp[ 0 ].get() );
				if( LOWORD( wParam ) == IDC_EDIT2 )
					doAVSEvalHighLight( hwndDlg, IDC_EDIT2, g_this->effect_exp[ 1 ].get() );
				if( LOWORD( wParam ) == IDC_EDIT3 )
					doAVSEvalHighLight( hwndDlg, IDC_EDIT3, g_this->effect_exp[ 2 ].get() );
				if( LOWORD( wParam ) == IDC_EDIT4 )
					doAVSEvalHighLight( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 3 ].get() );
#endif
			}
			if( LOWORD( wParam ) == IDC_DOT || LOWORD( wParam ) == IDC_LINES )
			{
				g_this->mode = IsDlgButtonChecked( hwndDlg, IDC_LINES ) ? 1 : 0;
			}
			if( LOWORD( wParam ) == IDC_WAVE || LOWORD( wParam ) == IDC_SPEC )
			{
				if( IsDlgButtonChecked( hwndDlg, IDC_WAVE ) ) g_this->which_ch &= ~4;
				else g_this->which_ch |= 4;
			}
			if( LOWORD( wParam ) == IDC_LEFTCH || LOWORD( wParam ) == IDC_RIGHTCH || LOWORD( wParam ) == IDC_MIDCH )
			{
				g_this->which_ch &= ~3;
				if( IsDlgButtonChecked( hwndDlg, IDC_LEFTCH ) );
				else if( IsDlgButtonChecked( hwndDlg, IDC_RIGHTCH ) ) g_this->which_ch |= 1;
				else g_this->which_ch |= 2;
			}
		}
		if( LOWORD( wParam ) == IDC_BUTTON2 )
		{
			char *text = "Superscope\0"
				"Superscope tutorial goes here\r\n"
				"But for now, here is the old text:\r\n"
				"You can specify expressions that run on Init, Frame, and on Beat.\r\n"
				"  'n' specifies the number of points to render (set this in Init, Beat, or Frame).\r\n"
				"For the 'Per Point' expression (which happens 'n' times per frame), use:\r\n"
				"  'x' and 'y' are the coordinates to draw to (-1..1)\r\n"
				"  'i' is the position of the scope (0..1)\r\n"
				"  'v' is the value at that point (-1..1).\r\n"
				"  'b' is 1 if beat, 0 if not.\r\n"
				"  'red', 'green' and 'blue' are all (0..1) and can be modified\r\n"
				"  'linesize' can be set from 1.0 to 255.0\r\n"
				"  'skip' can be set to >0 to skip drawing the current item\r\n"
				"  'drawmode' can be set to > 0 for lines, <= 0 for points\r\n"
				"  'w' and 'h' are the width and height of the screen, in pixels.\r\n"
				" Anybody want to send me better text to put here? Please :)\r\n"
				;

			compilerfunctionlist( hwndDlg, text );
		}
		if( LOWORD( wParam ) == IDC_BUTTON1 )
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
				SendMessage( hwndDlg, WM_COMMAND, MAKEWPARAM( IDC_EDIT4, EN_CHANGE ), 0 );
			}
			DestroyMenu( hMenu );
		}
		if( LOWORD( wParam ) == IDC_NUMCOL )
		{
			int p;
			BOOL tr = FALSE;
			p = GetDlgItemInt( hwndDlg, IDC_NUMCOL, &tr, FALSE );
			if( tr )
			{
				if( p > 16 ) p = 16;
				g_this->num_colors = p;
				InvalidateRect( GetDlgItem( hwndDlg, IDC_DEFCOL ), NULL, TRUE );
			}
		}
		if( LOWORD( wParam ) == IDC_DEFCOL )
		{
			int wc = -1, w, h;
			POINT p;
			RECT r;
			GetCursorPos( &p );
			GetWindowRect( GetDlgItem( hwndDlg, IDC_DEFCOL ), &r );
			p.x -= r.left;
			p.y -= r.top;
			w = r.right - r.left;
			h = r.bottom - r.top;
			if( p.x >= 0 && p.x < w  && p.y >= 0 && p.y < h )
			{
				wc = ( p.x*g_this->num_colors ) / w;
			}
			if( wc >= 0 )
			{
				GR_SelectColor( hwndDlg, g_this->colors + wc );
				InvalidateRect( GetDlgItem( hwndDlg, IDC_DEFCOL ), NULL, TRUE );
			}
		}
		return 0;
	}
	return 0;
}

HWND C_THISCLASS::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( IDD_CFG_SSCOPE ), hwndParent, g_DlgProc );
}