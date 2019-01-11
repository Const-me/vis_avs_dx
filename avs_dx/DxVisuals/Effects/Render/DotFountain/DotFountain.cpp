#include "stdafx.h"
#include "DotFountain.h"
#include <Resources/dynamicBuffers.h>

IMPLEMENT_EFFECT( DotFountain, C_DotFountainClass )

#include "Fountain.hlsli"

// Max.limit of the count, the size of the buffers
constexpr UINT totalParticles = NUM_ROT_DIV * NUM_ROT_HEIGHT;
constexpr UINT cbParticle = sizeof( FountainPoint );

DotFountain::DotFountain( AvsState *pState ) :
	FacticleFx( pState, cbParticle, totalParticles, "DotFountainUpdate" )
{
	initMatrix();
}

void DotFountain::onRenderSizeChanged()
{
	initMatrix();
}

#pragma warning( disable:4305 ) // : 'argument': truncation from 'double' to 'float'

void DotFountain::initMatrix()
{
	const Vector2 size = floatSize( getRenderSize() );
	const float aspect = size.x / size.y;
	const Matrix proj = Matrix::CreatePerspectiveFieldOfView( XMConvertToRadians( 60 ), aspect, 1, 5 );
	Matrix view = Matrix::CreateLookAt( -3 * Vector3::UnitY, Vector3::Zero, Vector3::UnitZ );
	m_viewProj = view * proj;
}

HRESULT DotFountain::render( bool isBeat, RenderTargets& rt )
{
	r += avs->rotvel / 5.0f;
	if( r >= 360.0f ) r -= 360.0f;
	if( r < 0.0f ) r += 360.0f;

	Matrix rot = Matrix::CreateFromYawPitchRoll( 0, XMConvertToRadians( (float)avs->angle ), XMConvertToRadians( r ) );
	Matrix wvp = rot * m_viewProj;
	wvp = wvp.Transpose();
	CHECK( updateCBuffer( m_cb, &wvp, sizeof( wvp ) ) );
	bindConstantBuffer<eStage::Vertex>( renderer.vertex().bindCBuffer, m_cb );

	omBlend( eBlend::Premultiplied );
	CHECK( rt.writeToLast( false ) );
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	return renderParticles( totalParticles );
}