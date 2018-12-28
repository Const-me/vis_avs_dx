#include "stdafx.h"
#include "Blitter.h"

IMPLEMENT_EFFECT( Blitter, C_BlitClass );

Blitter::Blitter( AvsState* avs ) : EffectBase1( avs )
{
	m_fade.update( 40 );
}

HRESULT Blitter::updateBuffer( float val )
{
	if( !m_cbuffer )
	{
		// Create
		CD3D11_BUFFER_DESC desc{ 16, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE };
		const float initBuffer[ 4 ] = { val, 0, 0, 0 };
		D3D11_SUBRESOURCE_DATA initData = { &initBuffer };
		CHECK( device->CreateBuffer( &desc, &initData, &m_cbuffer ) );
		m_val = val;
		return S_OK;
	}

	// Update
	if( m_val == val )
		return S_FALSE;

	D3D11_MAPPED_SUBRESOURCE mapped;
	CHECK( context->Map( m_cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) );
	float* dest = (float*)mapped.pData;
	*dest = val;
	context->Unmap( m_cbuffer, 0 );
	m_val = val;

	return S_OK;
}

inline float lerp( float x1, float x2, float x, float y1, float y2 )
{
	const float c1 = ( x - x1 ) / ( x2 - x1 );
	const float c2 = 1.0f - c1;
	return y1 * c2 + y2 * c1;
}

float Blitter::getScale( int s )
{
	if( s < 32 )
		return lerp( 32, 0, (float)s, 1, 2 );
	return lerp( 32, 255, (float)s, 1.0, 0.333f );
}

HRESULT Blitter::render( bool isBeat, RenderTargets& rt )
{
	constexpr UINT psSlot = 16;
	CHECK( rt.blendToNext( psSlot ) );

	float val = 0;
	if( avs->beatch )
		val = m_fade.value( isBeat );

	float zoom = lerp( 0, 1, val, getScale( avs->scale ), getScale( avs->scale2 ) );
	CHECK( updateBuffer( zoom ) );

	bindConstantBuffer<eStage::Vertex>( 3, m_cbuffer );
	renderer.bindShaders( false );

	if( avs->blend )
		omCustomBlend( 0.5 );
	else
		omBlend( eBlend::None );

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );

	bindConstantBuffer<eStage::Vertex>( 3 );
	bindResource<eStage::Pixel>( psSlot );

	return S_OK;
}