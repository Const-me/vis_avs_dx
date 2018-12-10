#include "stdafx.h"
#include "EffectsState.h"

HRESULT EffectsState::create( UINT totalSize )
{
	m_srv = nullptr;
	m_uav = nullptr;

	const UINT byteWidth = ( totalSize ) * 4;
	constexpr UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	CD3D11_BUFFER_DESC bufferDesc{ byteWidth, bindFlags, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS };

	CComPtr<ID3D11Buffer> buffer;
	CHECK( device->CreateBuffer( &bufferDesc, nullptr, &buffer ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_BUFFER, DXGI_FORMAT_R32_TYPELESS };
	CHECK( device->CreateShaderResourceView( buffer, &srvDesc, &m_srv ) );

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_BUFFER, DXGI_FORMAT_R32_TYPELESS };
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;	// Resource contains raw, unstructured data. Requires the UAV format to be DXGI_FORMAT_R32_TYPELESS.
	CHECK( device->CreateUnorderedAccessView( buffer, &uavDesc, &m_uav ) );

	return S_OK;
}