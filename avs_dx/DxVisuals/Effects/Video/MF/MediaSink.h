#pragma once
#include "mf.h"
#include "StreamSink.h"

class MediaSink :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMFMediaSink
{
public:
	BEGIN_COM_MAP( MediaSink )
		COM_INTERFACE_ENTRY( IMFMediaSink )
	END_COM_MAP()

private:

	// ==== IMFMediaSink methods ====
	HRESULT __stdcall GetCharacteristics( DWORD *pdwCharacteristics ) override;

	HRESULT __stdcall AddStreamSink( DWORD dwStreamSinkIdentifier, IMFMediaType *pMediaType, IMFStreamSink **ppStreamSink ) override;

	HRESULT __stdcall RemoveStreamSink( DWORD dwStreamSinkIdentifier ) override;

	HRESULT __stdcall GetStreamSinkCount( DWORD *pcStreamSinkCount ) override;

	HRESULT __stdcall GetStreamSinkByIndex( DWORD dwIndex, IMFStreamSink **ppStreamSink ) override;

	HRESULT __stdcall GetStreamSinkById( DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink ) override;

	HRESULT __stdcall SetPresentationClock( IMFPresentationClock *pPresentationClock ) override;

	HRESULT __stdcall GetPresentationClock( IMFPresentationClock **ppPresentationClock ) override;

	HRESULT __stdcall Shutdown() override;

	// ==== Implementation ====
	CComPtr<CComObject<StreamSink>> m_stream;
};