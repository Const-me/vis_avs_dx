#include "stdafx.h"
#include "../EffectImpl.hpp"
#include <Utils/FadeOut.h>
#include <Effects/shadersCode.h>

struct BlitterStructs
{
	struct AvsState
	{
		int scale, scale2, blend, beatch;
		int fpos;
		int subpixel;
	};

	using StateData = EmptyStateData;

	static ByteRange vertexShaderBinary()
	{
		return Hlsl::StaticShaders::BlitterVS();
	}
	static ByteRange pixelShaderBinary()
	{
		return Hlsl::StaticShaders::BlitterPS();
	}
};

inline float getScale( int s )
{
	if( s < 32 )
		return lerp( 32, 0, (float)s, 1, 2 );
	return lerp( 32, 255, (float)s, 1.0, 0.333f );
}

class Blitter : public EffectBase1<BlitterStructs>
{
	FadeOut m_fade;
	float m_val = 0;
	CComPtr<ID3D11Buffer> m_cbuffer;
	HRESULT updateBuffer( float val );

public:
	Blitter( AvsState* avs ) : EffectBase1( avs )
	{
		m_fade.update( 40 );
	}

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};

IMPLEMENT_EFFECT( Blitter, C_BlitClass )

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

HRESULT Blitter::render( bool isBeat, RenderTargets& rt )
{
	BoundPsResource bound;
	CHECK( rt.blendToNext( bound ) );

	float val = 0;
	if( avs->beatch )
		val = m_fade.value( isBeat );

	float zoom = lerp( 0, 1, val, getScale( avs->scale ), getScale( avs->scale2 ) );
	CHECK( updateBuffer( zoom ) );

	BIND_VS_CB( 3, m_cbuffer );
	renderer.bindShaders( false );

	if( avs->blend )
		omCustomBlend( 0.5 );
	else
		omBlend( eBlend::None );

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );

	return S_OK;
}