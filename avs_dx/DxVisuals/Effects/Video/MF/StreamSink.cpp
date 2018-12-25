#include "stdafx.h"
#include "StreamSink.h"

#define CHECK_SHUTDOWN if( wasShutdown() ) return MF_E_SHUTDOWN

HRESULT __stdcall StreamSink::GetMediaSink( IMFMediaSink **ppMediaSink )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall StreamSink::GetIdentifier( DWORD *pdwIdentifier )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall StreamSink::GetMediaTypeHandler( IMFMediaTypeHandler **ppHandler )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall StreamSink::ProcessSample( IMFSample *pSample )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall StreamSink::PlaceMarker( MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue )
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}

HRESULT __stdcall StreamSink::Flush()
{
	CHECK_SHUTDOWN;
	return E_NOTIMPL;
}