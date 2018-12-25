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
	if( m_clock )
		CHECK( m_clock->RemoveClockStateSink( this ) );
	m_clock = pPresentationClock;
	if( m_clock )
		CHECK( m_clock->AddClockStateSink( this ) );
	return S_OK;
}

HRESULT __stdcall MediaSink::GetPresentationClock( IMFPresentationClock **ppPresentationClock )
{
	CHECK_SHUTDOWN;
	CComPtr<IMFPresentationClock> clock = m_clock;
	if( !clock )
		return MF_E_NO_CLOCK;
	*ppPresentationClock = clock.Detach();
	return S_OK;
}

HRESULT __stdcall MediaSink::Shutdown()
{
	CHECK_SHUTDOWN;
	if( !m_stream )
	{
		return S_FALSE;
	}
	if( m_clock )
	{
		CHECK( m_clock->RemoveClockStateSink( this ) );
		m_clock = nullptr;
	}
	m_stream->shutdown();
	m_stream = nullptr;
	return S_OK;
}

HRESULT MediaSink::create( CComPtr<CComObject<MediaSink>>& mediaSinkObj, iSampleSink& sampleSink, IMFMediaTypeHandler* mth, CComPtr<IMFStreamSink>& streamSink )
{
	CComPtr<CComObject<MediaSink>> res;
	CHECK( createInstance( res ) );
	CHECK( res->initialize( sampleSink, mth, streamSink ) );
	mediaSinkObj = res;
	return S_OK;
}

HRESULT MediaSink::initialize( iSampleSink& sampleSink, IMFMediaTypeHandler* mth, CComPtr<IMFStreamSink>& streamSink )
{
	CHECK( createInstance( m_stream ) );
	CHECK( m_stream->initialize( this, sampleSink, mth ) );
	IMFStreamSink* pss = m_stream;
	streamSink = pss;
	return S_OK;
}

/* HRESULT MediaSink::requestSample()
{
	CHECK_SHUTDOWN;
	if( !m_stream )
		return OLE_E_BLANK;
	return m_stream->requestSample();
} */