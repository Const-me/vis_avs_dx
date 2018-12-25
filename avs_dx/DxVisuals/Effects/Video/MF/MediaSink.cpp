#include "stdafx.h"
#include "MediaSink.h"

#define CHECK_SHUTDOWN if( m_stream && m_stream->wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall MediaSink::GetCharacteristics( DWORD *pdwCharacteristics )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::AddStreamSink( DWORD dwStreamSinkIdentifier, IMFMediaType *pMediaType, IMFStreamSink **ppStreamSink )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::RemoveStreamSink( DWORD dwStreamSinkIdentifier )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::GetStreamSinkCount( DWORD *pcStreamSinkCount )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::GetStreamSinkByIndex( DWORD dwIndex, IMFStreamSink **ppStreamSink )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::GetStreamSinkById( DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::SetPresentationClock( IMFPresentationClock *pPresentationClock )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::GetPresentationClock( IMFPresentationClock **ppPresentationClock )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall MediaSink::Shutdown()
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}