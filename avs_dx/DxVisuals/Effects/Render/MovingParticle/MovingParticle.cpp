#include "stdafx.h"
#include "MovingParticle.h"
#include <Resources/dynamicBuffers.h>

IMPLEMENT_EFFECT( MovingParticle, C_BPartsClass )

MovingParticleStructs::sVertex MovingParticleStructs::AvsState::update( int& s_pos, bool isBeat )
{
	const CSize renderSize = getRenderSize();
	const int w = renderSize.cx;
	const int h = renderSize.cy;

	int xp, yp;
	int ss = min( h / 2, ( w * 3 ) / 8 );
	int oc6 = colors;

	if( isBeat )
	{
		c[ 0 ] = ( ( rand() % 33 ) - 16 ) / 48.0f;
		c[ 1 ] = ( ( rand() % 33 ) - 16 ) / 48.0f;
	}

	v[ 0 ] -= 0.004*( p[ 0 ] - c[ 0 ] );
	v[ 1 ] -= 0.004*( p[ 1 ] - c[ 1 ] );

	p[ 0 ] += v[ 0 ];
	p[ 1 ] += v[ 1 ];

	v[ 0 ] *= 0.991;
	v[ 1 ] *= 0.991;

	xp = (int)( p[ 0 ] * ( ss )*( maxdist / 32.0 ) ) + w / 2;
	yp = (int)( p[ 1 ] * ( ss )*( maxdist / 32.0 ) ) + h / 2;
	if( isBeat && ( enabled & 2 ) )
		s_pos = size2;
	int sz = s_pos;
	s_pos = ( s_pos + size ) / 2;

	sVertex res;
	res.position = Vector2( (float)xp, (float)yp );

	res.color = float3FromColor( colors );
	res.size = (float)sz;
	return res;
}

static const D3D11_INPUT_ELEMENT_DESC layoutDesc[ 2 ] =
{
	{ "SV_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

HRESULT MovingParticleStructs::VsData::compiledShader( const vector<uint8_t>& dxbc )
{
	CHECK( StaticResources::cacheInputLayout( layoutDesc, layoutDesc, 2, dxbc ) );
	return S_OK;
}

HRESULT MovingParticle::render( bool isBeat, RenderTargets& rt )
{
	if( !( avs->enabled & 1 ) )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	CHECK( rt.writeToLast( false ) );
	switch( avs->blend )
	{
	case 0:
	default:
		omBlend( eBlend::None );
		break;
	case 2:
		omCustomBlend();
		break;
	case 3:
		omBlend( eBlend::Add );
		break;
	}

	auto vb = avs->update( s_pos, isBeat );
	CHECK( updateVertexBuffer( m_vb, &vb, sizeof( vb ) ) );

	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	CHECK( StaticResources::bindCachedInputLayout( layoutDesc ) );
	iaSetVertexBuffer( m_vb, sizeof( vb ) );
	context->Draw( 1, 0 );
	return S_OK;
}