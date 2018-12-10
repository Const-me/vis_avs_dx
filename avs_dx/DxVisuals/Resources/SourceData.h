#pragma once

class SourceData
{
	static constexpr UINT bands = 576;

	// Texture of size bands * 4.
	// First two lines are spectrum, left/right channel, second two lines are wave.
	// Y texture coordinates: 0.125 left channel spectrum, 0.25 center channel spectrum, 0.375 right channel spectrum, 0.625 left channel waveform, 0.75 center channel waveform, 0.875 right channel waveform
	CComPtr<ID3D11Texture2D> m_texture;
	CComPtr<ID3D11ShaderResourceView> m_srv;

	struct sConstantBuffer
	{
		// Detect if it is currently a beat
		UINT isBeat;
		// Zero-based frame index since when the visualization started
		uint32_t currentFrame;
		// Time in milliseconds since last boot
		uint32_t getTickCount;
		uint32_t zzUnused;
	};

	CComPtr<ID3D11Buffer> m_cbuffer;

	uint32_t m_currentFrame = 0;

public:

	HRESULT create();

	void destroy();

	HRESULT update( char visdata[ 2 ][ 2 ][ bands ], int isBeat );
};