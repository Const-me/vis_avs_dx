#include "stdafx.h"
#include "MediaSink.h"

#define CHECK_SHUTDOWN if( m_stream && m_stream->wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall MediaSink::OnClockStart( MFTIME hnsSystemTime, LONGLONG llClockStartOffset )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::OnClockStop( MFTIME hnsSystemTime )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::OnClockPause( MFTIME hnsSystemTime )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::OnClockRestart( MFTIME hnsSystemTime )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::OnClockSetRate( MFTIME hnsSystemTime, float flRate )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}