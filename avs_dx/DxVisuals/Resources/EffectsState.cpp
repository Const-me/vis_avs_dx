#include "stdafx.h"
#include "EffectsState.h"

HRESULT EffectsState::create( UINT totalSize )
{
	m_buffer = nullptr;
	m_srv = nullptr;
	m_uav = nullptr;

	const UINT byteWidth = ( totalSize ) * 4;
	constexpr UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	UINT miscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	// D3D11 ERROR: ID3D11Device::CreateBuffer: The Dimensions are invalid. ByteWidth must be 12 or larger to be used with D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS. [ STATE_CREATION ERROR #66: CREATEBUFFER_INVALIDDIMENSIONS]
	if( totalSize >= 4 )
		miscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	CD3D11_BUFFER_DESC bufferDesc{ byteWidth, bindFlags, D3D11_USAGE_DEFAULT, 0, miscFlags };

	CHECK( device->CreateBuffer( &bufferDesc, nullptr, &m_buffer ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_BUFFEREX, DXGI_FORMAT_R32_TYPELESS };
	srvDesc.BufferEx.NumElements = totalSize;
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	CHECK( device->CreateShaderResourceView( m_buffer, &srvDesc, &m_srv ) );

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_BUFFER, DXGI_FORMAT_R32_TYPELESS };
	uavDesc.Buffer.NumElements = totalSize;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;	// Resource contains raw, unstructured data. Requires the UAV format to be DXGI_FORMAT_R32_TYPELESS.
	CHECK( device->CreateUnorderedAccessView( m_buffer, &uavDesc, &m_uav ) );

	m_size = totalSize;
	return S_OK;
}

void EffectsState::destroy()
{
	m_srv = nullptr;
	m_uav = nullptr;
	m_size = 0;
}