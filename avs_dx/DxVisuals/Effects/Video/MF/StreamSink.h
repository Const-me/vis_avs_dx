#pragma once
#include "EventGenerator.h"

class StreamSink:
	public CComObjectRootEx<CComMultiThreadModel>,
	public EventGenerator
{
public:

	BEGIN_COM_MAP( StreamSink )
		COM_INTERFACE_ENTRY( IMFMediaEventGenerator )
		COM_INTERFACE_ENTRY( IMFStreamSink )
	END_COM_MAP()

private:

	// ==== IMFStreamSink methods ====
	HRESULT __stdcall GetMediaSink( IMFMediaSink **ppMediaSink ) override;

	HRESULT __stdcall GetIdentifier( DWORD *pdwIdentifier ) override;

	HRESULT __stdcall GetMediaTypeHandler( IMFMediaTypeHandler **ppHandler ) override;

	HRESULT __stdcall ProcessSample( IMFSample *pSample ) override;

	HRESULT __stdcall PlaceMarker( MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue ) override;

	HRESULT __stdcall Flush();
};