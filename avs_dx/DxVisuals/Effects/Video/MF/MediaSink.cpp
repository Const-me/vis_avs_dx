#include "stdafx.h"
#include "MediaSink.h"

#define CHECK_SHUTDOWN if( m_stream && m_stream->wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall MediaSink::GetCharacteristics( DWORD *pdwCharacteristics )
{
	CHECK_SHUTDOWN;
	if( nullptr == pdwCharacteristics )
		return E_POINTER;
	constexpr DWORD flags = MEDIASINK_FIXED_STREAMS;	// TODO: add MEDIASINK_CAN_PREROLL and implement IMFMediaSinkPreroll interface
	*pdwCharacteristics = flags;
	return S_OK;
}

HRESULT __stdcall MediaSink::GetStreamSinkCount( DWORD *pcStreamSinkCount )
{
	CHECK_SHUTDOWN;
	if( nullptr == pcStreamSinkCount )
		return E_POINTER;
	*pcStreamSinkCount = 1;
	return S_OK;
}

HRESULT MediaSink::getStreamSink( DWORD dwIndex, IMFStreamSink **ppStreamSink )
{
	CHECK_SHUTDOWN;
	if( nullptr == ppStreamSink )
		return E_POINTER;
	if( 0 != dwIndex )
		return MF_E_INVALIDSTREAMNUMBER;

	CComPtr<IMFStreamSink> ss = m_stream.operator ->();
	*ppStreamSink = ss.Detach();
	return S_OK;
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