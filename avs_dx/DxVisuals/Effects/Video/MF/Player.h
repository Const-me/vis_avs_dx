#pragma once
#include "MediaSink.h"
#include "FrameTexture.h"

class Player
{
	CComPtr<IMFMediaSession> m_session;
	CComPtr<CComObject<MediaSink>> m_sink;
	FrameTexture m_texture;

	HRESULT createPlaybackTopology( IMFMediaSource* source, IMFPresentationDescriptor *pd, CComPtr<IMFTopology>& topology );

public:

	HRESULT start( LPCTSTR pathToVideo );
	HRESULT stop();

	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv )
	{
		return m_texture.getTexture( srv );
	}
};