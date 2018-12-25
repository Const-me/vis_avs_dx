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

// __stdcall so the compiler can implement GetStreamSinkByIndex/GetStreamSinkById with a single jmp, or even route both vtable entries to this method.
HRESULT __stdcall MediaSink::getStreamSink( DWORD i, IMFStreamSink **ppStreamSink )
{
	CHECK_SHUTDOWN;
	if( nullptr == ppStreamSink )
		return E_POINTER;
	if( 0 != i )
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

HRESULT MediaSink::create( CComPtr<CComObject<MediaSink>>& mediaSinkObj, iSampleSink& sampleSink, CComPtr<IMFStreamSink>& streamSink )
{
	CComPtr<CComObject<MediaSink>> res;
	CHECK( createInstance( res ) );
	CHECK( res->initialize( sampleSink, streamSink ) );
	mediaSinkObj = res;
	return S_OK;
}

HRESULT MediaSink::initialize( iSampleSink& sampleSink, CComPtr<IMFStreamSink>& streamSink )
{
	CHECK( createInstance( m_stream ) );
	CHECK( m_stream->initialize( this, sampleSink ) );
	streamSink = m_stream;
	return S_OK;
}