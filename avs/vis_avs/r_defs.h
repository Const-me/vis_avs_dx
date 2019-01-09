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
#ifndef _R_DEFS_H_
#define _R_DEFS_H_
#include <Interop/effectsFactory.h>
#include <Interop/msvcrt.h>
#include <Utils/logger.h>

// base class declaration, compatibility class
class RString;

class C_RBASE
{
public:
	C_RBASE() { }
	virtual ~C_RBASE() { };
	// Was abstract virtual
	virtual int render( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, int *framebuffer, int *fbout, int w, int h ) { return 0; };
	virtual HWND conf( HINSTANCE hInstance, HWND hwndParent ) { return 0; };
	virtual char *get_desc() = 0;
	virtual void load_config( unsigned char *data, int len ) { }
	virtual int save_config( unsigned char *data ) { return 0; }

	void load_string( RString &s, unsigned char *data, int &pos, int len );
	void save_string( unsigned char *data, int &pos, RString &text );

	void* operator new( size_t s )
	{
		return msvcrt::operatorNew( s );
	}
	void operator delete( void *ptr )
	{
		return msvcrt::operatorDelete( ptr );
	}
};

class C_RBASE2 : public C_RBASE {
public:
	C_RBASE2() { }
	virtual ~C_RBASE2() { };

	int getRenderVer2() { return 2; }

	virtual int smp_getflags() { return 0; } // return 1 to enable smp support

	// returns # of threads you desire, <= max_threads, or 0 to not do anything
	// default should return max_threads if you are flexible
	virtual int smp_begin( int max_threads, char visdata[ 2 ][ 2 ][ 576 ], int isBeat, int *framebuffer, int *fbout, int w, int h ) { return 0; }
	virtual void smp_render( int this_thread, int max_threads, char visdata[ 2 ][ 2 ][ 576 ], int isBeat, int *framebuffer, int *fbout, int w, int h ) { };
	virtual int smp_finish( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, int *framebuffer, int *fbout, int w, int h ) { return 0; }; // return value is that of render() for fbstuff etc
};


// defined in main.cpp, render.cpp
extern char g_path[];
extern int g_reset_vars_on_recompile;

// use this function to get a global buffer, and the last flag says whether or not to
// allocate it if it's not valid...
#define NBUF 8
void *getGlobalBuffer( int w, int h, int n, int do_alloc );


// implemented in util.cpp
void GR_SelectColor( HWND hwnd, int *a );
void GR_DrawColoredButton( DRAWITEMSTRUCT *di, COLORREF color );
void loadComboBox( HWND dlg, char *ext, char *selectedName );
void compilerfunctionlist( HWND hwndDlg, char *localinfo = NULL );

// matrix.cpp
void matrixRotate( float matrix[], char m, float Deg );
void matrixTranslate( float m[], float x, float y, float z );
void matrixMultiply( float *dest, float src[] );
void matrixApply( float *m, float x, float y, float z, float *outx, float *outy, float *outz );

// linedraw.cpp
extern int g_line_blend_mode;
void line( int *fb, int x1, int y1, int x2, int y2, int width, int height, int color, int lw );

inline unsigned int BLEND( unsigned int a, unsigned int b )
{
	return 0;
}

inline unsigned int BLEND_MAX( unsigned int a, unsigned int b )
{
	return 0;
}

inline unsigned int BLEND_MIN( unsigned int a, unsigned int b )
{
	return 0;
}

inline unsigned int BLEND_AVG( unsigned int a, unsigned int b )
{
	return 0;
}

inline unsigned int BLEND_SUB( unsigned int a, unsigned int b )
{
	return 0;
}

inline unsigned int BLEND_MUL( unsigned int a, unsigned int b )
{
	return 0;
}

static __inline void BLEND_LINE( int *fb, int color )
{ }

inline unsigned int BLEND_ADJ( unsigned int a, unsigned int b, int v )
{
	return 0;
}

inline unsigned int BLEND4( unsigned int *p1, unsigned int w, int xp, int yp )
{
	return 0;
}

inline unsigned int BLEND4_16( unsigned int *p1, unsigned int w, int xp, int yp )
{
	return 0;
}

inline void mmx_avgblend_block( int *output, int *input, int l )
{ }

inline void mmx_addblend_block( int *output, int *input, int l )
{ }

inline void mmx_mulblend_block( int *output, int *input, int l )
{ }

inline void mmx_adjblend_block( int *o, int *in1, int *in2, int len, int v )
{ }



class RString
{
public:
	RString() { m_str = 0; m_size = 0; }
	~RString() { if( m_str ) GlobalFree( m_str ); };
	void resize( int size ) { m_size = size; if( m_str ) GlobalFree( m_str ); m_str = 0; if( size ) m_str = (char*)GlobalAlloc( GPTR, size ); }
	char *get() { return m_str; }
	int getsize() { if( !m_str ) return 0; return m_size; }
	void assign( char *s ) { resize( strlen( s ) + 1 ); strcpy( m_str, s ); }
	void get_from_dlgitem( HWND hwnd, int dlgItem )
	{
		int l = SendDlgItemMessage( hwnd, dlgItem, WM_GETTEXTLENGTH, 0, 0 );
		if( l < 256 ) l = 256;
		resize( l + 1 + 256 );
		GetDlgItemTextA( hwnd, dlgItem, m_str, l + 1 );
		m_str[ l ] = 0;
	}
private:
	char *m_str;
	int m_size;
};

void doAVSEvalHighLight( HWND hwndDlg, UINT sub, char *data );

#endif