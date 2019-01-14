#include "stdafx.h"
#include "TempTexture.h"
#include <Interop/interop.h>

HRESULT TempTexture::update()
{
	const CSize rt = getRenderSize();
	const CSize size = { ( rt.cx + 7 ) / 8, ( rt.cy + 7 ) / 8 };
	if( size == m_size )
		return S_FALSE;

	m_size = size;
	m_srv = nullptr;
	m_uav = nullptr;

	constexpr DXGI_FORMAT fmt = DXGI_FORMAT_R16G16B16A16_UINT;
	CComPtr<ID3D11Texture2D> tex;
	const CD3D11_TEXTURE2D_DESC texDesc{ fmt, (UINT)size.cx, (UINT)size.cy, 1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &tex ) );

	const CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, fmt };
	CHECK( device->CreateShaderResourceView( tex, &srvDesc, &m_srv ) );

	const CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_TEXTURE2D, fmt };
	CHECK( device->CreateUnorderedAccessView( tex, &uavDesc, &m_uav ) );

	return S_OK;
}