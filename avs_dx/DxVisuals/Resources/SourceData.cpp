#include "stdafx.h"
#include "SourceData.h"
// Not only MFCopyImage simplifies the code, it's also very likely to be highly optimized.
#include <mfapi.h>
#pragma comment( lib, "Evr.lib" )
#include "../Utils/preciseTickCount.h"

static const double s_qpcMul = 1.0f / queryPerformanceFrequency();

HRESULT SourceData::create()
{
	static_assert( sizeof( sConstantBuffer ) % 16 == 0 );

	CD3D11_TEXTURE2D_DESC texDesc{ DXGI_FORMAT_R16_TYPELESS, bands, 4, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R16_SNORM };
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srvSigned ) );

	srvDesc.Format = DXGI_FORMAT_R16_UNORM;
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srvUnsigned ) );

	CD3D11_BUFFER_DESC bufferDesc{ sizeof( sConstantBuffer ), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
	CHECK( device->CreateBuffer( &bufferDesc, nullptr, &m_cbuffer ) );

	m_prevQpc = queryPerformanceCounter();

	ZeroMemory( &m_constantData, sizeof( m_constantData ) );

	return S_OK;
}

void SourceData::destroy()
{
	m_cbuffer = nullptr;
	m_srvSigned = m_srvUnsigned = nullptr;
	m_texture = nullptr;
}

HRESULT SourceData::update( uint16_t visdata[ 2 ][ 2 ][ bands ], int isBeat )
{
	D3D11_MAPPED_SUBRESOURCE mapped;

	// Update the texture
	{
		CHECK( context->Map( m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		const uint16_t* src = &visdata[ 0 ][ 0 ][ 0 ];
		uint8_t* dest = (uint8_t*)mapped.pData;
		CHECK( MFCopyImage( dest, mapped.RowPitch, (const BYTE*)src, bands * 2, bands * 2, 4 ) );
		context->Unmap( m_texture, 0 );
	}

	// Update the constants
	{
		m_constantData.isBeat = isBeat;
		m_constantData.currentFrame++;
		m_constantData.getTickCount = getPreciseTickCount();

		const uint64_t qpcNow = queryPerformanceCounter();
		m_constantData.deltaTime = (float)( ( qpcNow - m_prevQpc ) * s_qpcMul );
		m_prevQpc = qpcNow;

		m_constantData.lineModeAllowAlpha = TRUE;

		CHECK( context->Map( m_cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
		CopyMemory( mapped.pData, &m_constantData, sizeof( sConstantBuffer ) );
		context->Unmap( m_cbuffer, 0 );
	}

	return S_OK;
}

HRESULT SourceData::updateLineMode( bool allowAlpha )
{
	if( allowAlpha == !!m_constantData.lineModeAllowAlpha )
		return S_FALSE;

	m_constantData.lineModeAllowAlpha = allowAlpha;

	D3D11_MAPPED_SUBRESOURCE mapped;
	CHECK( context->Map( m_cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
	CopyMemory( mapped.pData, &m_constantData, sizeof( sConstantBuffer ) );
	context->Unmap( m_cbuffer, 0 );
	return S_OK;
}