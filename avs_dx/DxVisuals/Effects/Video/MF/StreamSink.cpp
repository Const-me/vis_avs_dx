#include "stdafx.h"
#include "StreamSink.h"
#pragma comment( lib, "Mfuuid.lib" )

HRESULT StreamSink::requestSample()
{
	return QueueEvent( MEStreamSinkRequestSample );
}

#define CHECK_SHUTDOWN if( wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall StreamSink::GetMediaSink( IMFMediaSink **ppMediaSink )
{
	CHECK_SHUTDOWN;
	if( nullptr == ppMediaSink )
		return E_POINTER;
	CComPtr<IMFMediaSink> sink = m_sink;
	if( !sink )
		return OLE_E_BLANK;
	*ppMediaSink = sink.Detach();
	return S_OK;
}

HRESULT __stdcall StreamSink::GetIdentifier( DWORD *pdwIdentifier )
{
	CHECK_SHUTDOWN;
	if( nullptr == pdwIdentifier )
		return E_POINTER;
	*pdwIdentifier = 0;
	return S_OK;
}

HRESULT __stdcall StreamSink::GetMediaTypeHandler( IMFMediaTypeHandler **ppHandler )
{
	CHECK_SHUTDOWN;
	if( nullptr == ppHandler )
		return E_POINTER;
	CComPtr<IMFMediaTypeHandler> mtHandler = m_mtHandler;
	if( !mtHandler )
		return OLE_E_BLANK;
	*ppHandler = mtHandler.Detach();
	return S_OK;
}

HRESULT __stdcall StreamSink::ProcessSample( IMFSample *pSample )
{
	CHECK_SHUTDOWN;
	iSampleSink *dest = m_pDest;
	if( dest != nullptr )
	{
		const HRESULT hr = dest->haveSample( pSample );
		if( FAILED( hr ) )
		{
			logWarning( hr, "iSampleSink::haveSample failed" );
		}
		if( hr == S_OK )
			CHECK( requestSample() );
	}
	return S_OK;
}

HRESULT __stdcall StreamSink::PlaceMarker( MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue )
{
	CHECK_SHUTDOWN;
	return S_OK;
}

HRESULT __stdcall StreamSink::Flush()
{
	CHECK_SHUTDOWN;
	if( !m_mtHandler )
		return MF_E_NOT_INITIALIZED;
	return S_OK;
}

HRESULT StreamSink::initialize( IMFMediaSink* owner, iSampleSink& sampleSink, IMFMediaTypeHandler* mth )
{
	CComPtr<IMFMediaType> mtSource;
	CHECK( mth->GetCurrentMediaType( &mtSource ) );

	uint64_t size, rate, aspect;
	CHECK( mtSource->GetUINT64( MF_MT_FRAME_SIZE, &size ) );
	CHECK( mtSource->GetUINT64( MF_MT_FRAME_RATE, &rate ) );
	CHECK( mtSource->GetUINT64( MF_MT_PIXEL_ASPECT_RATIO, &aspect ) );

	CHECK( MFCreateSimpleTypeHandler( &m_mtHandler ) );

	CComPtr<IMFMediaType> mt;
	CHECK( MFCreateMediaType( &mt ) );

	CHECK( mt->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Video ) );
	CHECK( mt->SetGUID( MF_MT_SUBTYPE, MFVideoFormat_RGB32 ) );

	// CHECK( mt->SetUINT64( MF_MT_FRAME_SIZE, size ) );
	// CHECK( mt->SetUINT64( MF_MT_FRAME_RATE, rate ) );
	// CHECK( mt->SetUINT64( MF_MT_PIXEL_ASPECT_RATIO, aspect ) );
	
	CHECK( m_mtHandler->SetCurrentMediaType( mt ) );

	CHECK( startEventGenerator() );

	m_sink = owner;
	m_pDest = &sampleSink;
	return S_OK;
}

void StreamSink::shutdown()
{
	m_sink = nullptr;
	m_mtHandler = nullptr;
	shutdownEventGenerator();
}