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
#include "render.h"
#include "undo.h"
#include "wnd.h"
#include "wa_ipc.h"

#ifdef LASER
C_LineListBase *g_laser_linelist;
#endif

C_RenderListClass *g_render_effects;
C_RenderListClass *g_render_effects2;
C_RenderTransitionClass *g_render_transition;
C_RLibrary *g_render_library;

void Render_Init( HINSTANCE hDllInstance )
{
	g_render_library = new C_RLibrary();
	g_render_effects = new C_RenderListClass( 1 );
	g_render_effects2 = new C_RenderListClass( 1 );
	g_render_transition = new C_RenderTransitionClass();

	char INI_FILE[ MAX_PATH ];
	char *p = INI_FILE;
	strncpy( INI_FILE, (char*)SendMessage( GetWinampHwnd(), WM_WA_IPC, 0, IPC_GETINIFILE ), MAX_PATH );
	p += strlen( INI_FILE ) - 1;
	while( p >= INI_FILE && *p != '\\' )
		p--;
	strcpy( p, "\\plugins\\vis_avs.dat" );

	extern int g_saved_preset_dirty;
	// clear the undo stack before loading a file.
	C_UndoStack::clear();
	g_render_effects->__LoadPreset( INI_FILE, 1 );
	// then add the new load to the undo stack but mark it clean if it is supposed to be
	C_UndoStack::saveundo();
	if( !g_saved_preset_dirty )
	{
		C_UndoStack::cleardirty();
	}
}

void Render_Quit( HINSTANCE hDllInstance )
{
	if( g_render_transition ) delete g_render_transition;
	g_render_transition = NULL;
	if( g_render_effects )
	{
		char INI_FILE[ MAX_PATH ];
		char *p = INI_FILE;
		strncpy( INI_FILE, (char*)SendMessage( GetWinampHwnd(), WM_WA_IPC, 0, IPC_GETINIFILE ), MAX_PATH );
		p += strlen( INI_FILE ) - 1;
		while( p >= INI_FILE && *p != '\\' ) p--;
#ifdef LASER
		strcpy( p, "\\plugins\\vis_avs_laser.dat" );
#else
		strcpy( p, "\\plugins\\vis_avs.dat" );
#endif

		g_render_effects->__SavePreset( INI_FILE );
	}

	if( g_render_effects )
	{
		destroyDxEffect( g_render_effects );
		delete g_render_effects;
	}
	g_render_effects = NULL;
	if( g_render_effects2 )
	{
		destroyDxEffect( g_render_effects2 );
		delete g_render_effects2;
	}
	g_render_effects2 = NULL;

	if( g_render_library ) delete g_render_library;
	g_render_library = NULL;
}