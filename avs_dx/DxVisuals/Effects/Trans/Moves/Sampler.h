#pragma once

class Sampler
{
	CComPtr<ID3D11SamplerState> m_sampler;
	bool m_bilinear;
	bool m_wrap;

public:

	HRESULT update( bool bilinear, bool wrap );

	operator ID3D11SamplerState* const( ) { return m_sampler; }
};