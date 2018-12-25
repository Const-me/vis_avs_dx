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

	static HRESULT create( CComPtr<CComObject<MediaSink>>& mediaSinkObj, iSampleSink& sampleSink, CComPtr<IMFStreamSink>& streamSink );

private:

	HRESULT initialize( iSampleSink& sampleSink, CComPtr<IMFStreamSink>& streamSink );

	// ==== IMFMediaSink methods ====
	HRESULT __stdcall GetCharacteristics( DWORD *pdwCharacteristics ) override;

	HRESULT __stdcall AddStreamSink( DWORD dwStreamSinkIdentifier, IMFMediaType *pMediaType, IMFStreamSink **ppStreamSink ) override { return MF_E_STREAMSINKS_FIXED; }

	HRESULT __stdcall RemoveStreamSink( DWORD dwStreamSinkIdentifier ) override { return MF_E_STREAMSINKS_FIXED; }

	HRESULT __stdcall GetStreamSinkCount( DWORD *pcStreamSinkCount ) override;

	HRESULT __stdcall getStreamSink( DWORD i, IMFStreamSink **ppStreamSink );

	HRESULT __stdcall GetStreamSinkByIndex( DWORD dwIndex, IMFStreamSink **ppStreamSink ) override
	{
		return getStreamSink( dwIndex, ppStreamSink );
	}

	HRESULT __stdcall GetStreamSinkById( DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink ) override
	{
		return getStreamSink( dwStreamSinkIdentifier, ppStreamSink );
	}

	HRESULT __stdcall SetPresentationClock( IMFPresentationClock *pPresentationClock ) override;

	HRESULT __stdcall GetPresentationClock( IMFPresentationClock **ppPresentationClock ) override;

	HRESULT __stdcall Shutdown() override;

	// ==== Implementation ====
	CComPtr<CComObject<StreamSink>> m_stream;
};