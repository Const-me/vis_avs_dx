#include "stdafx.h"
#include "MediaSink.h"

#define CHECK_SHUTDOWN if( m_stream && m_stream->wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall MediaSink::onClockStart( MFTIME hnsSystemTime, LONGLONG llClockStartOffset )
{
	CHECK_SHUTDOWN;
	if( !m_stream )
		return S_OK;

	CHECK( m_stream->QueueEvent( MEStreamSinkStarted ) );
	CHECK( m_stream->QueueEvent( MEStreamSinkRequestSample ) );
	return S_OK;
}

HRESULT __stdcall MediaSink::OnClockStop( MFTIME hnsSystemTime )
{
	CHECK_SHUTDOWN;
	if( m_stream )
		return m_stream->QueueEvent( MEStreamSinkStopped );
	return S_OK;
}

HRESULT __stdcall MediaSink::OnClockPause( MFTIME hnsSystemTime )
{
	CHECK_SHUTDOWN;
	if( m_stream )
		return m_stream->QueueEvent( MEStreamSinkPaused );
	return S_OK;
}

HRESULT __stdcall MediaSink::OnClockSetRate( MFTIME hnsSystemTime, float flRate )
{
	CHECK_SHUTDOWN;
	return S_OK;
}