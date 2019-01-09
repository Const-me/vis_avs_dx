#include "stdafx.h"
#include "Interference.h"
#include <Resources/dynamicBuffers.h>

IMPLEMENT_EFFECT( Interference, C_InterferencesClass )

InterferenceCb InterferenceStructs::AvsState::render( bool isBeat )
{
	InterferenceCb res = {};

	int pnts = nPoints;
	int i;
	int mask = 0;
	float s;

	float angle = (float)( 2 * M_PI ) / pnts;

	if( onbeat && isBeat )
		if( status >= M_PI )
			status = 0;

	s = sinf( status );
	float _rotationinc = (float)rotationinc + (float)( rotationinc2 - rotationinc ) * s;
	float _alpha = alpha + (float)( alpha2 - alpha ) * s;
	res.alpha.x = _alpha * div255;
	float _distance = distance + (float)( distance2 - distance ) * s;

	a = (float)rotation / 255 * (float)M_PI * 2;

	for( i = 0; i < pnts; i++ )
	{
		res.offsets[ i ].x = cosf( a ) * _distance;
		res.offsets[ i ].y = sinf( a ) * _distance;
		a += angle;
	}

	rotation += (int)_rotationinc;
	rotation = rotation > 255 ? rotation - 255 : rotation;
	rotation = rotation < -255 ? rotation + 255 : rotation;

	status += speed;
	status = eastl::min( status, (float)M_PI );
	if( status < -M_PI ) status = (float)M_PI;

	return res;
}

void Interference::bindResources()
{
	if( m_cb )
		bindConstantBuffer<eStage::Pixel>( renderer.pixel().bindConstBuffer, m_cb );
}

HRESULT Interference::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled || avs->nPoints <= 0 )
		return S_FALSE;

	const InterferenceCb cb = avs->render( isBeat );
	CHECK( updateCBuffer( m_cb, cb ) );
	bindResources();

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	BoundPsResource bound;
	if( avs->blendavg )
	{
		omCustomBlend();
		rt.blendToNext( bound );
	}
	else if(avs->blend)
	{
		omBlend( eBlend::Add );
		rt.blendToNext( bound );
	}
	else
	{
		omBlend( eBlend::None );
		rt.writeToNext( bound );
	}

	drawFullscreenTriangle( false );
	return S_OK;
}