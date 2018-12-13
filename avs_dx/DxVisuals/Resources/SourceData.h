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

	template<eStage stage>
	void bind( UINT t, UINT cb )
	{
		bindResource<stage>( t, m_srv );
		bindConstantBuffer<stage>( cb, m_cbuffer );
	}
};

enum struct eSource : uint8_t
{
	Spectrum = 0,
	Wave = 1
};
enum struct eChannel : uint8_t
{
	Left = 0,
	Center = 1,
	Right = 2
};
inline float sourceSampleV( eSource s, eChannel ch )
{
	constexpr float sources[ 2 ] = { 0.25, 0.75 };
	constexpr float channels[ 3 ] = { -0.125f, 0, +0.125 };
	return sources[ (uint8_t)s ] + channels[ (uint8_t)ch ];
}