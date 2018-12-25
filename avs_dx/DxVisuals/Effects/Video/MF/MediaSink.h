#pragma once
#include "mf.h"
#include "StreamSink.h"

class MediaSink :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IMFMediaSink,
	public IMFClockStateSink
{
public:
	BEGIN_COM_MAP( MediaSink )
		COM_INTERFACE_ENTRY( IMFMediaSink )
		COM_INTERFACE_ENTRY( IMFClockStateSink )
	END_COM_MAP()

	static HRESULT create( CComPtr<CComObject<MediaSink>>& mediaSinkObj, iSampleSink& sampleSink, CComPtr<IMFStreamSink>& streamSink );

	// HRESULT requestSample();

	HRESULT __stdcall Shutdown() override;

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

	// ==== IMFClockStateSink methods ====
	HRESULT __stdcall onClockStart( MFTIME hnsSystemTime, LONGLONG llClockStartOffset );

	HRESULT __stdcall OnClockStart( MFTIME hnsSystemTime, LONGLONG llClockStartOffset ) override { return onClockStart( hnsSystemTime, llClockStartOffset ); }

	HRESULT __stdcall OnClockStop( MFTIME hnsSystemTime ) override;

	HRESULT __stdcall OnClockPause( MFTIME hnsSystemTime ) override;

	HRESULT __stdcall OnClockRestart( MFTIME hnsSystemTime ) override { return onClockStart( hnsSystemTime, 0 ); }

	HRESULT __stdcall OnClockSetRate( MFTIME hnsSystemTime, float flRate ) override;

	// ==== Implementation ====
	CComPtr<CComObject<StreamSink>> m_stream;
	CComPtr<IMFPresentationClock> m_clock;
};