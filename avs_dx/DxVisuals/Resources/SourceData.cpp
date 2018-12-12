#include "stdafx.h"
#include "SourceData.h"
// Not only MFCopyImage simplifies the code, it's also very likely to be highly optimized.
#include <mfapi.h>
#pragma comment( lib, "Evr.lib" )
#include "../Utils/preciseTickCount.h"

HRESULT SourceData::create()
{
	static_assert( sizeof( sConstantBuffer ) % 16 == 0 );

	constexpr DXGI_FORMAT texFormat = DXGI_FORMAT_R8_SNORM;

	CD3D11_TEXTURE2D_DESC texDesc{ texFormat, bands, 4, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, texFormat };
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srv ) );

	CD3D11_BUFFER_DESC bufferDesc{ sizeof( sConstantBuffer ), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
	CHECK( device->CreateBuffer( &bufferDesc, nullptr, &m_cbuffer ) );

	return S_OK;
}

void SourceData::destroy()
{
	m_cbuffer = nullptr;
	m_srv = nullptr;
	m_texture = nullptr;
	m_currentFrame = 0;
}

HRESULT SourceData::update( char visdata[ 2 ][ 2 ][ bands ], int isBeat )
{
	D3D11_MAPPED_SUBRESOURCE mapped;

	{
		CHECK( context->Map( m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		const char* src = &visdata[ 0 ][ 0 ][ 0 ];
		uint8_t* dest = (uint8_t*)mapped.pData;
		CHECK( MFCopyImage( dest, mapped.RowPitch, (const BYTE*)src, bands, bands, 4 ) );
		context->Unmap( m_texture, 0 );
	}

	{
		CHECK( context->Map( m_cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		sConstantBuffer* dest = (sConstantBuffer*)mapped.pData;
		dest->isBeat = isBeat;
		dest->currentFrame = m_currentFrame;
		dest->getTickCount = getPreciseTickCount();
		context->Unmap( m_cbuffer, 0 );

		m_currentFrame++;
	}

	return S_OK;
}