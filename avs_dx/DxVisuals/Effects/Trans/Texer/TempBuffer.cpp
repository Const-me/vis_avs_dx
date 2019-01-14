#include "stdafx.h"
#include "TempBuffer.h"

HRESULT TempBuffer::create()
{
	if( m_uav )
		return S_FALSE;

	constexpr UINT capacity = 1024;

	CD3D11_BUFFER_DESC bufferDesc{ sizeof( Vector2 ) * capacity, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	bufferDesc.StructureByteStride = sizeof( Vector2 );
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	CComPtr<ID3D11Buffer> buffer;
	CHECK( device->CreateBuffer( &bufferDesc, nullptr, &buffer ) );

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_BUFFER, DXGI_FORMAT_UNKNOWN };
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = capacity;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	CHECK( device->CreateUnorderedAccessView( buffer, &uavDesc, &m_uav ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_BUFFER, DXGI_FORMAT_UNKNOWN };
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = capacity;

	CHECK( device->CreateShaderResourceView( buffer, &srvDesc, &m_srv ) );

	return S_OK;
}