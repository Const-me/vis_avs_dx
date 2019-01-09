#include "stdafx.h"
#include "AvsThreads.h"
#include <Interop/Utils/dbgSetThreadName.h>
#include "GuiThread.h"
#include "RenderThread.h"
#include  <Utils/DynamicArray.hpp>
#include <Interop/avsUtils.h>

AvsThreads& getThreads()
{
	static AvsThreads g_threads;
	return g_threads;
}

template<class T>
DWORD __stdcall AvsThreads::threadMain( void* ppvDest )
{
	dbgSetThreadName( T::dbgGetThreadName() );

	T threadData{ getThreads().this_mod };

	return (DWORD)getThreads().threadMain2( threadData, (ThreadBase **)ppvDest );
}

HRESULT AvsThreads::threadMain2( ThreadBase& tb, ThreadBase **ppDest )
{
	{
		const HRESULT hr = tb.initialize();
		if( FAILED( hr ) )
			return hr;
	}

	{
		CSLock __lock( m_csRendering );
		*ppDest = &tb;
	}

	SetEvent( evtLaunched );

	const HRESULT hr = tb.run();

	{
		CSLock __lock( m_csRendering );
		*ppDest = nullptr;
	}
	return hr;
}

#undef CHECK
#define CHECK( hr, msg )        {  const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }
#define FAIL_LAST_WIN32( msg )  {  const HRESULT __hr = getLastHr(); if( FAILED( __hr ) ) { logError( __hr, msg ); return __hr; }  }

HRESULT AvsThreads::ThreadInfo::launch( LPTHREAD_START_ROUTINE fnMain, ThreadBase **ppDest, HANDLE hLaunchedEvent )
{
	if( hThread )
	{
		assert( false );
		return HRESULT_FROM_WIN32( ERROR_ALREADY_INITIALIZED );
	}

	ResetEvent( hLaunchedEvent );

	DWORD tid = 0;
	hThread.Attach( ::CreateThread( nullptr, 0, fnMain, ppDest, 0, &tid ) );
	if( !hThread )
		FAIL_LAST_WIN32( "CreateThread failed" );

	const HANDLE handles[ 2 ] = { hLaunchedEvent, hThread };

	const DWORD res = WaitForMultipleObjects( 2, handles, FALSE, 2500 );

	if( res == WAIT_OBJECT_0 )
		return S_OK;

	switch( res )
	{
	case WAIT_OBJECT_0 + 1:
	{
		DWORD ec;
		GetExitCodeThread( hThread, &ec );
		assert( FAILED( ec ) );
		return HRESULT( ec );
	}
	case WAIT_TIMEOUT:
		return E_TIMEOUT;
	}
	return getLastHr();
}

HRESULT AvsThreads::launchBothThreads()
{
	evtLaunched.Attach( ::CreateEvent( nullptr, TRUE, FALSE, nullptr ) );
	if( !evtLaunched )
		FAIL_LAST_WIN32( "CreateEvent failed" );

	CHECK( m_gui.launch( &AvsThreads::threadMain<GuiThreadImpl>, (ThreadBase**)&m_pGui, evtLaunched ), "Error launching GUI thread" );

	CHECK( m_render.launch( &AvsThreads::threadMain<RenderThreadImpl>, (ThreadBase**)&m_pRender, evtLaunched ), "Error launching GUI thread" );

	evtLaunched.Close();

	return S_OK;
}

HRESULT AvsThreads::start( winampVisModule *pMod )
{
	this_mod = pMod;
	{
		const HRESULT hr = launchBothThreads();
		if( FAILED( hr ) )
			stop();
		return hr;
	}

	return S_OK;
}

HRESULT AvsThreads::stop()
{
	BoolHr result;
	auto st = [ &result, this ]( ThreadBase** ppThread, CHandle& h )
	{
		{
			CSLock __lock( m_csRendering );
			if( nullptr == *ppThread )
				return;
			( *ppThread )->postQuitmessage();
		}
		assert( h );
		result.combine( msgWaitForSingleObject( h, INFINITE ) );
		h.Close();
		result = S_OK;
	};

	st( (ThreadBase**)&m_pRender, m_render.hThread );
	st( (ThreadBase**)&m_pGui, m_gui.hThread );
	return result;
}