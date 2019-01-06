#include "stdafx.h"
#include "AddBorder.h"
#include "borderHacks.hpp"
#include <Resources/dynamicBuffers.h>

IMPLEMENT_APE_EFFECT( AddBorder, "Virtual Effect: Addborders", "AddBorder.ape" );



AddBorder::AddBorder( C_RBASE* pThis ) :
	EffectBase1( pThis )
{ }

Vector4 AddBorder::SourceData::getConstantBuffer() const
{
	return float4FromColor( color, (float)size * ( 1.0f / 50.0f ) );
}

AddBorder::SourceData AddBorder::queryData() const
{
	SourceData res;
	res.color = BorderHacks::getColor( avs );
	res.size = BorderHacks::getWidth( avs );
	return res;
}

HRESULT AddBorder::ensureIndexBuffer()
{
	if( m_ib )
		return S_FALSE;

	// 0-------------------2
	// | \         ----  / |
	// |  \   ----      /  |
	// |   1-----------3   |
	// |   |           |   |
	// |   |           |   |

	std::array<std::array<uint16_t, 3>, 8> ibData;
	for( uint16_t i = 0; i < 4; i++ )
	{
		const uint16_t outerThis = i * 2;
		const uint16_t innerThis = i * 2 + 1;
		const uint16_t nc = ( i + 1 ) % 4;
		const uint16_t outerNext = nc * 2;
		const uint16_t innernext = nc * 2 + 1;
		ibData[ i * 2 ] = std::array<uint16_t, 3>{outerThis, innerThis, outerNext };
		ibData[ i * 2 + 1 ] = std::array<uint16_t, 3>{outerNext, innerThis, innernext };
	}

	CD3D11_BUFFER_DESC desc{ sizeof( ibData ), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE };
	D3D11_SUBRESOURCE_DATA srd{ ibData.data(), 0,0 };
	CHECK( device->CreateBuffer( &desc, &srd, &m_ib ) );

	return S_OK;
}

HRESULT AddBorder::render( bool isBeat, RenderTargets& rt )
{
	if( !BorderHacks::isEnabled( avs ) )
		return S_FALSE;

	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	Vector4 cb = queryData().getConstantBuffer();
	CHECK( updateCBuffer( m_cb, &cb, sizeof( cb ) ) );

	bindConstantBuffer<eStage::Vertex>( renderer.vertex().bindConstBuffer, m_cb );
	bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );

	CHECK( ensureIndexBuffer() );

	omBlend( eBlend::None );
	iaClearBuffer();
	context->IASetIndexBuffer( m_ib, DXGI_FORMAT_R16_UINT, 0 );
	context->DrawIndexed( 3 * 4 * 2, 0, 0 );
	return S_FALSE;
}