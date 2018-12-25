#pragma once
#include "playerApi.h"
#include "EventListener.h"
#include "MediaSink.h"
#include "FrameTexture.h"
#include "EventSlim.h"

class Player:
	public CComObjectRootEx<CComMultiThreadModel>,
	public EventListener,
	public iPlayer
{
	CComPtr<IMFMediaSession> m_session;
	CComPtr<CComObject<MediaSink>> m_sink;
	FrameTexture m_texture;
	EventSlim m_evtClosed;

	HRESULT createPlaybackTopology( IMFMediaSource* source, IMFPresentationDescriptor *pd, CComPtr<IMFTopology>& topology );

	HRESULT stop() override;

	HRESULT onEvent( MediaEventType eventType, HRESULT hrStatus ) override;

	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv ) override
	{
		return m_texture.getTexture( srv );
	}

public:

	BEGIN_COM_MAP( Player )
		COM_INTERFACE_ENTRY( IMFAsyncCallback )
	END_COM_MAP()

	HRESULT start( LPCTSTR pathToVideo );
};