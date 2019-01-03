#include "stdafx.h"
#include "dynamicBuffers.h"

HRESULT updateCBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb )
{
	if( ptr )
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		CHECK( context->Map( ptr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		CopyMemory( mapped.pData, pData, cb );
		context->Unmap( ptr, 0 );
		return S_OK;
	}
	else
	{
		D3D11_SUBRESOURCE_DATA data{ pData, 0, 0 };
		CD3D11_BUFFER_DESC bufferDesc{ (UINT)cb, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
		CHECK( device->CreateBuffer( &bufferDesc, &data, &ptr ) );
		return S_OK;
	}
}