#include "stdafx.h"
#include "resizeHandler.h"

namespace
{
	CAtlMap<iResizeHandler*, bool> g_resizeHandlers;
	CComAutoCriticalSection g_cs;
}

void subscribeHandler( iResizeHandler* pHandler )
{
	CSLock __lock( g_cs );
	g_resizeHandlers[ pHandler ] = true;
}

void unsubscribeHandler( iResizeHandler* pHandler )
{
	CSLock __lock( g_cs );
	g_resizeHandlers.RemoveKey( pHandler );
}

void callResizeHandlers()
{
	CSLock __lock( g_cs );
	for( POSITION pos = g_resizeHandlers.GetStartPosition(); nullptr != pos; )
	{
		iResizeHandler* p = g_resizeHandlers.GetNextKey( pos );
		p->onRenderSizeChanged();
	}
}

ResizeHandler::ResizeHandler()
{
	subscribeHandler( this );
}

ResizeHandler::~ResizeHandler()
{
	unsubscribeHandler( this );
}