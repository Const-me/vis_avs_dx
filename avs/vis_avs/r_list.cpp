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
#include <stdio.h>
#include <commctrl.h>
#include "resource.h"
#include "r_defs.h"
#include "r_unkn.h"
#include "r_list.h"
#include "render.h"
#include "undo.h"

#include "avs_eelif.h"

#define PUT_INT(y) data[pos]=(y)&255; data[pos+1]=(y>>8)&255; data[pos+2]=(y>>16)&255; data[pos+3]=(y>>24)&255
#define GET_INT() (data[pos]|(data[pos+1]<<8)|(data[pos+2]<<16)|(data[pos+3]<<24))

char *C_RenderListClass::get_desc() { if( isroot ) return "Main"; return "Effect list"; }

static char extsigstr[] = "AVS 2.8+ Effect List Config";

void C_RenderListClass::load_config_code( unsigned char *data, int len )
{
	int pos = 0;
	if( len - pos >= 4 )
	{
		use_code = GET_INT(); pos += 4;
		load_string( effect_exp[ 0 ], data, pos, len );
		load_string( effect_exp[ 1 ], data, pos, len );
	}

}

void C_RenderListClass::load_config( unsigned char *data, int len )
{
	int pos = 0, ext;
	if( pos < len ) { mode = data[ pos++ ]; }
	if( mode & 0x80 )
	{
		mode &= ~0x80;
		mode = mode | GET_INT(); pos += 4;
	}
	ext = get_extended_datasize() + 5;
	if( ext > 5 )
	{
		if( pos < ext ) { inblendval = GET_INT(); pos += 4; }
		if( pos < ext ) { outblendval = GET_INT(); pos += 4; }
		if( pos < ext ) { bufferin = GET_INT(); pos += 4; }
		if( pos < ext ) { bufferout = GET_INT(); pos += 4; }
		if( pos < ext ) { ininvert = GET_INT(); pos += 4; }
		if( pos < ext ) { outinvert = GET_INT(); pos += 4; }
		if( pos < ext - 4 ) { beat_render = GET_INT(); pos += 4; }	//BU
		if( pos < ext - 4 ) { beat_render_frames = GET_INT(); pos += 4; }
	}
	use_code = 0;
	effect_exp[ 0 ].assign( "" );
	effect_exp[ 1 ].assign( "" );
	while( pos < len )
	{
		char s[ 33 ];
		T_RenderListType t;
		int l_len;
		t.effect_index = GET_INT(); pos += 4;
		if( t.effect_index >= DLLRENDERBASE )
		{
			if( pos + 32 > len ) break;
			memcpy( s, data + pos, 32 );
			s[ 32 ] = 0;
			t.effect_index = (int)s;
			pos += 32;
		}
		if( pos + 4 > len ) break;
		l_len = GET_INT(); pos += 4;
		if( pos + l_len > len || l_len < 0 ) break;

		if( ext > 5 && t.effect_index >= DLLRENDERBASE && !memcmp( s, extsigstr, strlen( extsigstr ) + 1 ) )
		{
			load_config_code( data + pos, l_len );
		}
		else
		{
			t.render = g_render_library->CreateRenderer( &t.effect_index, &t.has_rbase2 );
			if( t.render )
			{
				t.render->load_config( data + pos, l_len );
				insertRender( &t, -1 );
			}
		}
		pos += l_len;
	}
}

int  C_RenderListClass::save_config_code( unsigned char *data )
{
	int pos = 0;
	PUT_INT( use_code ); pos += 4;
	save_string( data, pos, effect_exp[ 0 ] );
	save_string( data, pos, effect_exp[ 1 ] );
	return pos;
}

int  C_RenderListClass::save_config( unsigned char *data )
{
	return save_config_ex( data, 0 );
}
int  C_RenderListClass::save_config_ex( unsigned char *data, int rootsave )
{
	int pos = 0;
	int x;
	if( !rootsave )
	{
		set_extended_datasize( 36 ); // size of extended data + 4 cause we fucked up
		data[ pos++ ] = ( mode & 0xff ) | 0x80;
		PUT_INT( mode ); pos += 4;
		// extended_data
		PUT_INT( inblendval ); pos += 4;
		PUT_INT( outblendval ); pos += 4;
		PUT_INT( bufferin ); pos += 4;
		PUT_INT( bufferout ); pos += 4;
		PUT_INT( ininvert ); pos += 4;
		PUT_INT( outinvert ); pos += 4;
		PUT_INT( beat_render ); pos += 4;
		PUT_INT( beat_render_frames ); pos += 4;
		// end extended data
	}
	else data[ pos++ ] = mode;

	if( !rootsave )
	{
		// write in our ext field
		PUT_INT( DLLRENDERBASE ); pos += 4;
		char s[ 33 ];
		strncpy( s, extsigstr, 32 );
		memcpy( data + pos, s, 32 );
		pos += 32;
		int t = save_config_code( data + pos + 4 );
		PUT_INT( t );
		pos += 4 + t;
	}


	for( x = 0; x < num_renders; x++ )
	{
		int t;
		int idx = renders[ x ].effect_index;
		if( idx == UNKN_ID )
		{
			C_UnknClass *r = (C_UnknClass *)renders[ x ].render;
			if( !r->idString[ 0 ] ) { PUT_INT( r->id ); pos += 4; }
			else
			{
				PUT_INT( r->id ); pos += 4;
				memcpy( data + pos, r->idString, 32 );
				pos += 32;
			}
		}
		else
		{
			PUT_INT( idx );	pos += 4;
			if( idx >= DLLRENDERBASE )
			{
				char s[ 33 ];
				strncpy( s, (char*)idx, 32 );
				memcpy( data + pos, s, 32 );
				pos += 32;
			}
		}

		t = renders[ x ].render->save_config( data + pos + 4 );
		PUT_INT( t );
		pos += 4 + t;
	}



	return pos;
}


C_RenderListClass::C_RenderListClass( int iroot )
{
	AVS_EEL_INITINST();
	isstart = 0;
#ifndef LASER
	nsaved = 0;
	memset( nbw_save, 0, sizeof( nbw_save ) );
	memset( nbw_save2, 0, sizeof( nbw_save2 ) );
	memset( nbh_save, 0, sizeof( nbh_save ) );
	memset( nbh_save2, 0, sizeof( nbh_save2 ) );
	memset( nb_save, 0, sizeof( nb_save ) );
	memset( nb_save2, 0, sizeof( nb_save2 ) );
#endif
	inblendval = 128;
	outblendval = 128;
	ininvert = 0;

	InitializeCriticalSection( &rcs );
	use_code = 0;
	inited = 0;
	need_recompile = 1;
	memset( codehandle, 0, sizeof( codehandle ) );
	var_beat = 0;


	effect_exp[ 0 ].assign( "" );
	effect_exp[ 1 ].assign( "" );
	outinvert = 0;
	bufferin = 0;
	bufferout = 0;
	isroot = iroot;
	num_renders = 0;
	num_renders_alloc = 0;
	renders = NULL;
	thisfb = NULL;
	l_w = l_h = 0;
	mode = 0;
	beat_render = 0;
	beat_render_frames = 1;
	fake_enabled = 0;
#ifdef LASER
	if( !iroot ) line_save = createLineList();
	else line_save = NULL;
#endif
	CREATE_DX_EFFECT( thisfb );
}

extern int g_n_buffers_w[ NBUF ], g_n_buffers_h[ NBUF ];
extern void *g_n_buffers[ NBUF ];

#ifndef LASER
void C_RenderListClass::set_n_Context()
{
	if( !isroot ) return;
	if( nsaved ) return;
	nsaved = 1;
	memcpy( nbw_save2, g_n_buffers_w, sizeof( nbw_save2 ) );
	memcpy( nbh_save2, g_n_buffers_h, sizeof( nbh_save2 ) );
	memcpy( nb_save2, g_n_buffers, sizeof( nb_save2 ) );

	memcpy( g_n_buffers_w, nbw_save, sizeof( nbw_save ) );
	memcpy( g_n_buffers_h, nbh_save, sizeof( nbh_save ) );
	memcpy( g_n_buffers, nb_save, sizeof( nb_save ) );
}

void C_RenderListClass::unset_n_Context()
{
	if( !isroot ) return;
	if( !nsaved ) return;
	nsaved = 0;

	memcpy( nbw_save, g_n_buffers_w, sizeof( nbw_save ) );
	memcpy( nbh_save, g_n_buffers_h, sizeof( nbh_save ) );
	memcpy( nb_save, g_n_buffers, sizeof( nb_save ) );

	memcpy( g_n_buffers_w, nbw_save2, sizeof( nbw_save2 ) );
	memcpy( g_n_buffers_h, nbh_save2, sizeof( nbh_save2 ) );
	memcpy( g_n_buffers, nb_save2, sizeof( nb_save2 ) );

}
#endif

void C_RenderListClass::smp_cleanupthreads()
{

	if( smp_parms.threadTop > 0 )
	{
		if( smp_parms.hQuitHandle ) SetEvent( smp_parms.hQuitHandle );

		WaitForMultipleObjects( smp_parms.threadTop, smp_parms.hThreads, TRUE, INFINITE );
		int x;
		for( x = 0; x < smp_parms.threadTop; x++ )
		{
			CloseHandle( smp_parms.hThreads[ x ] );
			CloseHandle( smp_parms.hThreadSignalsDone[ x ] );
			CloseHandle( smp_parms.hThreadSignalsStart[ x ] );
		}
	}

	if( smp_parms.hQuitHandle ) CloseHandle( smp_parms.hQuitHandle );

	memset( &smp_parms, 0, sizeof( smp_parms ) );
}

void C_RenderListClass::freeBuffers()
{
#ifndef LASER
	if( isroot )
	{
		int x;
		for( x = 0; x < NBUF; x++ )
		{
			if( nb_save[ x ] ) GlobalFree( nb_save[ x ] );
			nb_save[ x ] = NULL;
			nbw_save[ x ] = nbh_save[ x ] = 0;
		}
	}
#endif
}

C_RenderListClass::~C_RenderListClass()
{
#ifdef LASER
	if( line_save ) delete line_save;
#endif
	clearRenders();

	// free nb_save
	freeBuffers();

	int x;
	for( x = 0; x < 2; x++ )
	{
		freeCode( codehandle[ x ] );
		codehandle[ x ] = 0;
	}
	AVS_EEL_QUITINST();
	DeleteCriticalSection( &rcs );
}

static int __inline depthof( int c, int i )
{
	int r = max( max( ( c & 0xFF ), ( ( c & 0xFF00 ) >> 8 ) ), ( c & 0xFF0000 ) >> 16 );
	return i ? 255 - r : r;
}

int C_RenderListClass::getNumRenders( void )
{
	return num_renders;
}

C_RenderListClass::T_RenderListType *C_RenderListClass::getRender( int index )
{
	if( index >= 0 && index < num_renders ) return &renders[ index ];
	return NULL;
}

int C_RenderListClass::findRender( T_RenderListType *r )
{
	int idx;
	if( !r ) return -1;
	for( idx = 0; idx < num_renders&&renders[ idx ].render != r->render; idx++ );
	if( idx < num_renders ) return idx;
	return -1;
}

int C_RenderListClass::removeRenderFrom( T_RenderListType *r, int del )
{
	int idx;
	if( !r ) return 1;
	for( idx = 0; idx < num_renders&&renders[ idx ].render != r->render; idx++ );
	return removeRender( idx, del );
}

int C_RenderListClass::removeRender( int index, int del )
{
	if( index >= 0 && index < num_renders )
	{
		if( del&&renders[ index ].render ) delete renders[ index ].render;
		num_renders--;
		while( index < num_renders )
		{
			renders[ index ] = renders[ index + 1 ];
			index++;
		}
		if( !num_renders )
		{
			num_renders_alloc = 0;
			if( renders ) GlobalFree( (HGLOBAL)renders );
			renders = NULL;
		}
		return 0;
	}
	return 1;
}
void C_RenderListClass::clearRenders( void )
{
	int x;
	if( renders )
	{
		for( x = 0; x < num_renders; x++ )
		{
			delete renders[ x ].render;
		}
		GlobalFree( (HGLOBAL)renders );
	}
	num_renders = 0;
	num_renders_alloc = 0;
	renders = NULL;
	if( thisfb ) GlobalFree( (HGLOBAL)thisfb );
	thisfb = 0;
	if( dxEffect )
		dxEffect->clearRenders();
}

int C_RenderListClass::insertRenderBefore( T_RenderListType *r, T_RenderListType *before )
{
	int idx;
	if( !before ) idx = num_renders;
	else for( idx = 0; idx < num_renders&&renders[ idx ].render != before->render; idx++ );
	return insertRender( r, idx );
}

int C_RenderListClass::insertRender( T_RenderListType *r, int index ) // index=-1 for add
{
	if( num_renders + 1 >= num_renders_alloc || !renders )
	{
		num_renders_alloc = num_renders + 16;
		T_RenderListType *newr = (T_RenderListType *)GlobalAlloc( GPTR, num_renders_alloc * sizeof( T_RenderListType ) );
		if( !newr ) return -1;
		if( num_renders&&renders )
		{
			memcpy( newr, renders, num_renders * sizeof( T_RenderListType ) );
		}
		if( renders )
		{
			GlobalFree( (HGLOBAL)renders );
		}
		renders = newr;
	}

	if( index < 0 || index >= num_renders )
	{
		renders[ num_renders ] = *r;
		return num_renders++;
	}
	int x;
	for( x = num_renders++; x > index; x-- )
	{
		renders[ x ] = renders[ x - 1 ];
	}
	renders[ x ] = *r;

	return x;
}

void C_RenderListClass::FillBufferCombo( HWND dlg, int ctl )
{
	int i = 0;
	char txt[ 64 ];
	for( i = 0; i < NBUF; i++ )
	{
		wsprintf( txt, "Buffer %d", i + 1 );
		SendDlgItemMessage( dlg, ctl, CB_ADDSTRING, 0, (int)txt );
	}
}

static C_RenderListClass *g_this;
static const char *blendmodes[] =
{
  "Ignore",
  "Replace",
  "50/50",
  "Maximum",
  "Additive",
  "Subtractive 1",
  "Subtractive 2",
  "Every other line",
  "Every other pixel",
  "XOR",
	"Adjustable",
  "Multiply",
	"Buffer",
  "Minimum",
};

BOOL CALLBACK C_RenderListClass::g_DlgProcRoot( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
#ifdef LASER
		ShowWindow( GetDlgItem( hwndDlg, IDC_CHECK1 ), SW_HIDE );
#endif
		{
#ifndef LASER
			if( g_this->clearfb() ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
#endif
		}
		return 1;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_CHECK1:
			g_this->set_clearfb( IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) );
			break;
		}
		break;
	}
	return 0;
}


BOOL CALLBACK C_RenderListClass::g_DlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
#ifdef LASER
		ShowWindow( GetDlgItem( hwndDlg, IDC_CHECK1 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_INSLIDE ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_OUTSLIDE ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF1 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF2 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT1 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT2 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_COMBO1 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_COMBO2 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_FR1 ), SW_HIDE );
		ShowWindow( GetDlgItem( hwndDlg, IDC_FR2 ), SW_HIDE );
#endif
		{
#ifndef LASER
			{
				int x;
				for( x = 0; x < sizeof( blendmodes ) / sizeof( blendmodes[ 0 ] ); x++ )
				{
					SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)blendmodes[ x ] );
					SendDlgItemMessage( hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)blendmodes[ x ] );
				}
				SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_SETCURSEL, (WPARAM)g_this->blendout(), 0 );
				SendDlgItemMessage( hwndDlg, IDC_COMBO2, CB_SETCURSEL, (WPARAM)g_this->blendin(), 0 );
				ShowWindow( GetDlgItem( hwndDlg, IDC_INSLIDE ), ( g_this->blendin() == 10 ) ? SW_NORMAL : SW_HIDE );
				ShowWindow( GetDlgItem( hwndDlg, IDC_OUTSLIDE ), ( g_this->blendout() == 10 ) ? SW_NORMAL : SW_HIDE );
				SendDlgItemMessage( hwndDlg, IDC_INSLIDE, TBM_SETRANGE, TRUE, MAKELONG( 0, 255 ) );
				SendDlgItemMessage( hwndDlg, IDC_INSLIDE, TBM_SETPOS, TRUE, (int)( g_this->inblendval ) );
				SendDlgItemMessage( hwndDlg, IDC_OUTSLIDE, TBM_SETRANGE, TRUE, MAKELONG( 0, 255 ) );
				SendDlgItemMessage( hwndDlg, IDC_OUTSLIDE, TBM_SETPOS, TRUE, (int)( g_this->outblendval ) );
				ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF1 ), ( g_this->blendin() == 12 ) ? SW_NORMAL : SW_HIDE );
				ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF2 ), ( g_this->blendout() == 12 ) ? SW_NORMAL : SW_HIDE );
				ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT1 ), ( g_this->blendin() == 12 ) ? SW_NORMAL : SW_HIDE );
				ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT2 ), ( g_this->blendout() == 12 ) ? SW_NORMAL : SW_HIDE );
				g_this->FillBufferCombo( hwndDlg, IDC_CBBUF1 );
				g_this->FillBufferCombo( hwndDlg, IDC_CBBUF2 );
				SendDlgItemMessage( hwndDlg, IDC_CBBUF1, CB_SETCURSEL, (WPARAM)g_this->bufferin, 0 );
				SendDlgItemMessage( hwndDlg, IDC_CBBUF2, CB_SETCURSEL, (WPARAM)g_this->bufferout, 0 );
				if( g_this->ininvert ) CheckDlgButton( hwndDlg, IDC_INVERT1, BST_CHECKED );
				if( g_this->outinvert ) CheckDlgButton( hwndDlg, IDC_INVERT2, BST_CHECKED );
			}
			if( g_this->clearfb() ) CheckDlgButton( hwndDlg, IDC_CHECK1, BST_CHECKED );
#endif
			g_this->isstart = 1;
			SetDlgItemText( hwndDlg, IDC_EDIT4, g_this->effect_exp[ 0 ].get() );
			SetDlgItemText( hwndDlg, IDC_EDIT5, g_this->effect_exp[ 1 ].get() );
			g_this->isstart = 0;


			if( ( ( g_this->mode & 2 ) ^ 2 ) )
				CheckDlgButton( hwndDlg, IDC_CHECK2, BST_CHECKED );
			if( g_this->beat_render ) CheckDlgButton( hwndDlg, IDC_CHECK3, BST_CHECKED );
			else EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), FALSE );
			if( g_this->use_code ) CheckDlgButton( hwndDlg, IDC_CHECK4, BST_CHECKED );
			char buf[ 999 ];
			wsprintf( buf, "%d", g_this->beat_render_frames );
			SetDlgItemText( hwndDlg, IDC_EDIT1, buf );
		}
		return 1;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_EDIT4:
		case IDC_EDIT5:
			if( !g_this->isstart && HIWORD( wParam ) == EN_CHANGE )
			{
				EnterCriticalSection( &g_this->rcs );
				g_this->effect_exp[ 0 ].get_from_dlgitem( hwndDlg, IDC_EDIT4 );
				g_this->effect_exp[ 1 ].get_from_dlgitem( hwndDlg, IDC_EDIT5 );
				g_this->need_recompile = 1;
				if( LOWORD( wParam ) == IDC_EDIT4 ) g_this->inited = 0;
				LeaveCriticalSection( &g_this->rcs );
			}
			break;
		case IDC_COMBO1:
			if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				int r = SendDlgItemMessage( hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0 );
				if( r != CB_ERR )
				{
					g_this->set_blendout( r );
					ShowWindow( GetDlgItem( hwndDlg, IDC_OUTSLIDE ), ( r == 10 ) ? SW_NORMAL : SW_HIDE );
					ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF2 ), ( g_this->blendout() == 12 ) ? SW_NORMAL : SW_HIDE );
					ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT2 ), ( g_this->blendout() == 12 ) ? SW_NORMAL : SW_HIDE );
				}
			}
			break;
		case IDC_COMBO2:
			if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				int r = SendDlgItemMessage( hwndDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0 );
				if( r != CB_ERR )
				{
					g_this->set_blendin( r );
					ShowWindow( GetDlgItem( hwndDlg, IDC_INSLIDE ), ( r == 10 ) ? SW_NORMAL : SW_HIDE );
					ShowWindow( GetDlgItem( hwndDlg, IDC_CBBUF1 ), ( g_this->blendin() == 12 ) ? SW_NORMAL : SW_HIDE );
					ShowWindow( GetDlgItem( hwndDlg, IDC_INVERT1 ), ( g_this->blendin() == 12 ) ? SW_NORMAL : SW_HIDE );
				}
			}
			break;
		case IDC_CBBUF1:
			if( HIWORD( wParam ) == CBN_SELCHANGE )
				g_this->bufferin = SendDlgItemMessage( hwndDlg, IDC_CBBUF1, CB_GETCURSEL, 0, 0 );
			break;
		case IDC_CBBUF2:
			if( HIWORD( wParam ) == CBN_SELCHANGE )
				g_this->bufferout = SendDlgItemMessage( hwndDlg, IDC_CBBUF2, CB_GETCURSEL, 0, 0 );
			break;
		case IDC_CHECK1:
			g_this->set_clearfb( IsDlgButtonChecked( hwndDlg, IDC_CHECK1 ) );
			break;
		case IDC_CHECK2:
			g_this->set_enabled( IsDlgButtonChecked( hwndDlg, IDC_CHECK2 ) );
			break;
		case IDC_INVERT1:
			g_this->ininvert = IsDlgButtonChecked( hwndDlg, IDC_INVERT1 );
			break;
		case IDC_INVERT2:
			g_this->outinvert = IsDlgButtonChecked( hwndDlg, IDC_INVERT2 );
			break;
		case IDC_CHECK3:
			g_this->beat_render = IsDlgButtonChecked( hwndDlg, IDC_CHECK3 );
			EnableWindow( GetDlgItem( hwndDlg, IDC_EDIT1 ), g_this->beat_render );
			break;
		case IDC_CHECK4:
			g_this->use_code = !!IsDlgButtonChecked( hwndDlg, IDC_CHECK4 );
			break;
		case IDC_EDIT1:
			if( HIWORD( wParam ) == EN_CHANGE ) {
				char buf[ 999 ] = "1";
				GetDlgItemText( hwndDlg, IDC_EDIT1, buf, 999 );
				buf[ 998 ] = 0;
				g_this->beat_render_frames = atoi( buf );
			}
			break;
		case IDC_BUTTON2:
		{
			char *text = "Effect List\0"
				"Read/write 'enabled' to get/set whether the effect list is enabled for this frame\r\n"
				"Read/write 'beat' to get/set whether there is currently a beat\r\n"
				"Read/write 'clear' to get/set whether to clear the framebuffer\r\n"
				"If the input blend is set to adjustable, 'alphain' can be set from 0.0-1.0\r\n"
				"If the output blend is set to adjustable, 'alphaout' can be set from 0.0-1.0\r\n"
				"'w' and 'h' are set with the current width and height of the frame\r\n"
				;

			compilerfunctionlist( hwndDlg, text );
		}
		break;
		}
		break;
	case WM_NOTIFY:
		if( LOWORD( wParam ) == IDC_INSLIDE )
			g_this->inblendval = SendDlgItemMessage( hwndDlg, IDC_INSLIDE, TBM_GETPOS, 0, 0 );
		if( LOWORD( wParam ) == IDC_OUTSLIDE )
			g_this->outblendval = SendDlgItemMessage( hwndDlg, IDC_OUTSLIDE, TBM_GETPOS, 0, 0 );
		break;
	}
	return 0;
}


HWND C_RenderListClass::conf( HINSTANCE hInstance, HWND hwndParent )
{
	g_this = this;
	return CreateDialog( hInstance, MAKEINTRESOURCE( isroot ? IDD_CFG_LISTROOT : IDD_CFG_LIST ), hwndParent, isroot ? g_DlgProcRoot : g_DlgProc );
	//  return NULL;
}

char C_RenderListClass::sig_str[] = "Nullsoft AVS Preset 0.2\x1a";

int C_RenderListClass::__SavePreset( char *filename )
{
	EnterCriticalSection( &g_render_cs );
	unsigned char *data = (unsigned char *)GlobalAlloc( GPTR, 1124 * 1024 );
	int success = -1;
	if( data )
	{
		int pos = 0;
		memcpy( data + pos, sig_str, strlen( sig_str ) ); pos += strlen( sig_str );
		pos += save_config_ex( data + pos, 1 );
		if( pos < 1024 * 1024 )
		{
			HANDLE fp = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			if( fp != INVALID_HANDLE_VALUE )
			{
				DWORD dw;
				success = 0;
				WriteFile( fp, data, pos, &dw, NULL );
				CloseHandle( fp );
			}
			else success = 2;
		}
		else success = 1;
		GlobalFree( (HGLOBAL)data );
	}
	LeaveCriticalSection( &g_render_cs );
	return success;
}

int C_RenderListClass::__LoadPreset( char *filename, int clear )
{
	EnterCriticalSection( &g_render_cs );
	unsigned char *data = (unsigned char *)GlobalAlloc( GPTR, 1024 * 1024 );
	int success = 1;
	if( clear ) clearRenders();
	if( data )
	{
		//  OutputDebugString(filename);
		HANDLE fp = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( fp != INVALID_HANDLE_VALUE )
		{
			DWORD len = GetFileSize( fp, NULL );
			if( len == 0xffffffff ) len = 0;
			if( !ReadFile( fp, data, min( len, 1024 * 1024 ), &len, NULL ) ) len = 0;
			CloseHandle( fp );
			if( len > strlen( sig_str ) + 2 && !memcmp( data, sig_str, strlen( sig_str ) - 2 ) &&
				data[ strlen( sig_str ) - 2 ] >= '1' &&
				data[ strlen( sig_str ) - 2 ] <= '2' &&
				data[ strlen( sig_str ) - 1 ] == '\x1a' )
			{
				load_config( data + strlen( sig_str ), len - strlen( sig_str ) );
				success = 0;
			}
			else
			{
				//     if (len<=strlen(sig_str)+2) MessageBox(NULL,"Error laoding preset: len",filename,MB_OK);
			  //       else MessageBox(NULL,"Error laoding preset: signature",filename,MB_OK);
			}
		}
		//  else MessageBox(NULL,"Error laoding preset: fopen",filename,MB_OK);
		GlobalFree( (HGLOBAL)data );
	}
	//  else MessageBox(NULL,"Error laoding preset: MALLOC",filename,MB_OK);
	LeaveCriticalSection( &g_render_cs );
	return success;
}

int C_RenderListClass::__SavePresetToUndo( C_UndoItem &item )
{
	EnterCriticalSection( &g_render_cs );
	unsigned char *data = (unsigned char *)GlobalAlloc( GPTR, 1124 * 1024 );
	int success = -1;
	if( data )
	{
		// Do whatever the file saving stuff did
		int pos = 0;
		memcpy( data + pos, sig_str, strlen( sig_str ) ); pos += strlen( sig_str );
		pos += save_config_ex( data + pos, 1 );

		// And then set the data into the undo object.
		if( pos < 1024 * 1024 )
		{
			item.set( data, pos, true ); // all undo items start dirty.
		}

		else success = 1;
		GlobalFree( (HGLOBAL)data );
	}
	LeaveCriticalSection( &g_render_cs );
	return success;
}

int C_RenderListClass::__LoadPresetFromUndo( C_UndoItem &item, int clear )
{
	EnterCriticalSection( &g_render_cs );
	unsigned char *data = (unsigned char *)GlobalAlloc( GPTR, 1024 * 1024 );
	int success = 1;
	if( clear ) clearRenders();
	if( data )
	{
		if( item.size() < 1024 * 1024 )
		{
			// Get the data from the undo object.
			DWORD len = item.size();
			if( len == 0xffffffff ) len = 0;
			memcpy( data, item.get(), item.size() );

			// And then do whatever the file loading stuff did.
			if( !memcmp( data, sig_str, strlen( sig_str ) - 2 ) &&
				data[ strlen( sig_str ) - 2 ] >= '1' &&
				data[ strlen( sig_str ) - 2 ] <= '2' &&
				data[ strlen( sig_str ) - 1 ] == '\x1a' )
			{
				load_config( data + strlen( sig_str ), len - strlen( sig_str ) );
				success = 0;
			}
		}
		GlobalFree( (HGLOBAL)data );
	}
	LeaveCriticalSection( &g_render_cs );
	return success;
}




/// smp fun

void C_RenderListClass::smp_Render( int minthreads, C_RBASE2 *render, char visdata[ 2 ][ 2 ][ 576 ], int isBeat, int *framebuffer, int *fbout, int w, int h )
{
	int x;
	smp_parms.nthreads = minthreads;
	if( !smp_parms.hQuitHandle ) smp_parms.hQuitHandle = CreateEvent( NULL, TRUE, FALSE, NULL );

	smp_parms.vis_data_ptr = visdata;
	smp_parms.isBeat = isBeat;
	smp_parms.framebuffer = framebuffer;
	smp_parms.fbout = fbout;
	smp_parms.w = w;
	smp_parms.h = h;
	smp_parms.render = render;
	for( x = 0; x < minthreads; x++ )
	{
		if( x >= smp_parms.threadTop )
		{
			DWORD id;
			smp_parms.hThreadSignalsStart[ x ] = CreateEvent( NULL, FALSE, TRUE, NULL );
			smp_parms.hThreadSignalsDone[ x ] = CreateEvent( NULL, FALSE, FALSE, NULL );

			smp_parms.hThreads[ x ] = CreateThread( NULL, 0, smp_threadProc, (LPVOID)( x ), 0, &id );
			smp_parms.threadTop = x + 1;
		}
		else
			SetEvent( smp_parms.hThreadSignalsStart[ x ] );
	}
	WaitForMultipleObjects( smp_parms.nthreads, smp_parms.hThreadSignalsDone, TRUE, INFINITE );
}

DWORD WINAPI C_RenderListClass::smp_threadProc( LPVOID parm )
{
	int which = (int)parm;
	HANDLE hdls[ 2 ] = { smp_parms.hThreadSignalsStart[ which ],smp_parms.hQuitHandle };
	for( ;;)
	{
		if( WaitForMultipleObjects( 2, hdls, FALSE, INFINITE ) == WAIT_OBJECT_0 + 1 ) return 0;

		smp_parms.render->smp_render( which, smp_parms.nthreads,
			*( char( *)[ 2 ][ 2 ][ 576 ] )smp_parms.vis_data_ptr,

			smp_parms.isBeat, smp_parms.framebuffer, smp_parms.fbout, smp_parms.w, smp_parms.h );
		SetEvent( smp_parms.hThreadSignalsDone[ which ] );
	}
}

C_RenderListClass::_s_smp_parms C_RenderListClass::smp_parms;