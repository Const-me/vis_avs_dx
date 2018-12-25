#include "stdafx.h"
#include "EventGenerator.h"
#pragma comment( lib, "Mfplat.lib" )

bool EventGenerator::wasShutdown() const
{
	return m_shutdown;
}

#define CHECK_SHUTDOWN if( wasShutdown() ) return MF_E_SHUTDOWN

HRESULT EventGenerator::startEventGenerator()
{
	CHECK_SHUTDOWN;
	if( nullptr != m_queue )
	{
		logError( "EventGenerator::startup called more than once" );
		return E_FAIL;
	}

	CHECK( MFCreateEventQueue( &m_queue ) );
	return S_OK;
}

void EventGenerator::shutdownEventGenerator()
{
	m_shutdown = true;
	if( m_queue )
		m_queue->Shutdown();
}

HRESULT __stdcall EventGenerator::GetEvent( DWORD dwFlags, IMFMediaEvent **ppEvent )
{
	CHECK_SHUTDOWN;
	// GetEvent is a blocking call.
	// AddRef into a local variable to guarantee the MEQ will stay alive until this method returns even if some other thread is destroying things.
	CComPtr<IMFMediaEventQueue> pQueue = m_queue;
	if( !pQueue )
		return E_POINTER;
	const HRESULT hr = pQueue->GetEvent( dwFlags, ppEvent );
	if( SUCCEEDED( hr ) )
		dbgLogMediaEvent( "StreamSink sync", *ppEvent );
	return hr;
}

HRESULT __stdcall EventGenerator::BeginGetEvent( IMFAsyncCallback *pCallback, IUnknown *punkState )
{
	CHECK_SHUTDOWN;
	return m_queue->BeginGetEvent( pCallback, punkState );
}

HRESULT __stdcall EventGenerator::EndGetEvent( IMFAsyncResult *pResult, IMFMediaEvent **ppEvent )
{
	CHECK_SHUTDOWN;
	const HRESULT hr = m_queue->EndGetEvent( pResult, ppEvent );
	if( SUCCEEDED( hr ) )
		dbgLogMediaEvent( "StreamSink async", *ppEvent );
	return hr;
}

HRESULT __stdcall EventGenerator::QueueEvent( MediaEventType met, const GUID &guidExtendedType, HRESULT hrStatus, const PROPVARIANT *pvValue )
{
	CHECK_SHUTDOWN;
	return m_queue->QueueEventParamVar( met, guidExtendedType, hrStatus, pvValue );
}