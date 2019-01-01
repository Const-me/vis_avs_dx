#include "stdafx.h"
#include "Sampler.h"

HRESULT Sampler::update( bool bilinear, bool wrap )
{
	if( m_sampler && bilinear == m_bilinear && wrap == m_wrap )
	{
		return S_FALSE;
	}

	CD3D11_SAMPLER_DESC sd{ D3D11_DEFAULT };
	sd.Filter = bilinear ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
	const D3D11_TEXTURE_ADDRESS_MODE a = wrap ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressU = a;
	sd.AddressV = a;

	m_sampler = nullptr;
	CHECK( device->CreateSamplerState( &sd, &m_sampler ) );

	m_bilinear = bilinear;
	m_wrap = wrap;
	return S_OK;
}