#pragma once
#include "iSampleSink.h"

// Consumes video samples on background threads, retains the last frame's MF texture, then on rendering thread copies the frames into a normal texture that has a view.
class FrameTexture: public iSampleSink
{
	// Consume video sample
	HRESULT haveSample( IMFSample* sample ) override;

	CComAutoCriticalSection m_cs;
	CComPtr<ID3D11Texture2D> m_texture;

	CComPtr<ID3D11Texture2D> m_viewTexture;
	CComPtr<ID3D11ShaderResourceView> m_srv;

public:

	void destroy();

	// Copy last frame to the normal texture, return shader resource view of that.
	HRESULT getTexture( CComPtr<ID3D11ShaderResourceView>& srv );
};