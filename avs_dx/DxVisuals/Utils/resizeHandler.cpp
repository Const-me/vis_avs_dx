#include "stdafx.h"
#include "resizeHandler.h"

namespace
{
	class Handlers
	{
		CAtlMap<iResizeHandler*, bool> m_handlers;
		CComAutoCriticalSection m_cs;

	public:

		void subscribe( iResizeHandler* pHandler )
		{
			CSLock __lock( m_cs );
			m_handlers[ pHandler ] = true;
		}

		void unsubscribe( iResizeHandler* pHandler )
		{
			CSLock __lock( m_cs );
			m_handlers.RemoveKey( pHandler );
		}

		void invoke()
		{
			CSLock __lock( m_cs );
			for( POSITION pos = m_handlers.GetStartPosition(); nullptr != pos; )
			{
				iResizeHandler* p = m_handlers.GetNextKey( pos );
				p->onRenderSizeChanged();
			}
		}
	};

	static Handlers& handlers()
	{
		// This is to be able to subscribe in static constructors, as C++ doesn't guarantee initialization order across source files.
		static Handlers s_handlers;
		return s_handlers;
	}
}

void subscribeHandler( iResizeHandler* pHandler )
{
	handlers().subscribe( pHandler );
}

void unsubscribeHandler( iResizeHandler* pHandler )
{
	handlers().unsubscribe( pHandler );
}

void callResizeHandlers()
{
	handlers().invoke();
}

ResizeHandler::ResizeHandler()
{
	subscribeHandler( this );
}

ResizeHandler::~ResizeHandler()
{
	unsubscribeHandler( this );
}