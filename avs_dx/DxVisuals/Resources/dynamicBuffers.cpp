#include "stdafx.h"
#include "dynamicBuffers.h"

namespace
{
	HRESULT updateBuffer( ID3D11Resource *pResource, const void* pData, size_t cb )
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		CHECK( context->Map( pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		CopyMemory( mapped.pData, pData, cb );
		context->Unmap( pResource, 0 );
		return S_OK;
	}

	HRESULT createDynamicBuffer( UINT bindFlags, const void* pData, size_t cb, ID3D11Buffer **ppBuffer )
	{
		D3D11_SUBRESOURCE_DATA data{ pData, 0, 0 };
		CD3D11_BUFFER_DESC bufferDesc{ (UINT)cb, bindFlags, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
		CHECK( device->CreateBuffer( &bufferDesc, &data, ppBuffer ) );
		return S_OK;
	}
}

HRESULT updateCBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb )
{
	if( ptr )
		return updateBuffer( ptr, pData, cb );
	return createDynamicBuffer( D3D11_BIND_CONSTANT_BUFFER, pData, cb, &ptr );
}

HRESULT updateVertexBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb )
{
	if( ptr )
		return updateBuffer( ptr, pData, cb );
	return createDynamicBuffer( D3D11_BIND_VERTEX_BUFFER, pData, cb, &ptr );
}