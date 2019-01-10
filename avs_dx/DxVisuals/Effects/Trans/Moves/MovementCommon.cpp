#include "stdafx.h"
#include "MovementCommon.h"

HRESULT MovementStructs::CommonStateData::updateScreenSize()
{
	return updateInput( screenSize, getRenderSize() );
}

HRESULT MovementStructs::CommonVsData::compiledShader( const vector<uint8_t>& dxbc )
{
	return StaticResources::createLayout( dxbc );
}

HRESULT MovementStructs::CommonVsData::updateAvs( bool rectCoords )
{
	const CSize rs = getRenderSize();
	Vector2 diag{ (float)rs.cx, (float)rs.cy };
	diag.Normalize();

	BoolHr res;
	res.updateValue( scaleToUniform, diag );
	res.updateValue( rectangularCoords, rectCoords );
	return res;
}

HRESULT MovementFx::render( RenderTargets& rt, bool bilinear, bool wrap, UINT samplerSlot, bool blend, bool rectCoords )
{
	CHECK( m_sampler.update( bilinear, wrap ) );
	BIND_PS_SAMPLER( samplerSlot, m_sampler );

	BoundPsResource bound;
	if( blend )
	{
		CHECK( rt.blendToNext( bound ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( bound ) );
		omBlend( eBlend::None );
	}
	CHECK( m_mesh.draw( rectCoords ) );
	return S_OK;
}

HRESULT MovementFx::render( RenderTargets& rt, bool bilinear, bool wrap, UINT samplerSlot, bool blend, bool rectCoords, int sourceBuffer )
{
	if( 0 == sourceBuffer )
		return render( rt, bilinear, wrap, samplerSlot, blend, rectCoords );

	CHECK( m_sampler.update( bilinear, wrap ) );
	BIND_PS_SAMPLER( samplerSlot, m_sampler );
	RenderTarget& src = StaticResources::globalBuffers[ sourceBuffer - 1 ].lastWritten();

	CHECK( rt.writeToLast( false ) );

	auto sourceSrv = src.srv();
	if( nullptr == sourceSrv )
		sourceSrv = StaticResources::blackTexture;
	BoundPsResource bound{ 3, sourceSrv };

	if( blend )
		omCustomBlend( 0.5f );
	else
		omBlend( eBlend::None );

	CHECK( m_mesh.draw( rectCoords ) );
	return S_OK;
}