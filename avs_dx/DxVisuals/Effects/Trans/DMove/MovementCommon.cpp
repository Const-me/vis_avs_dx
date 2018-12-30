#include "stdafx.h"
#include "MovementCommon.h"

HRESULT MovementStructs::CommonStateData::updateScreenSize()
{
	return updateInput( screenSize, getRenderSize() );
}

HRESULT MovementStructs::CommonVsData::compiledShader( const std::vector<uint8_t>& dxbc )
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

HRESULT MovementFx::render( RenderTargets& rt, bool bilinear, bool wrap, UINT psReadSlot, bool blend, bool rectCoords )
{
	CHECK( m_sampler.update( bilinear, wrap ) );
	BIND_PS_SAMPLER( 1, m_sampler );

	BoundPsResource psRead;
	if( blend )
	{
		CHECK( rt.blendToNext( psReadSlot, psRead ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( psReadSlot, psRead, false ) );
		omBlend( eBlend::None );
	}
	CHECK( m_mesh.draw( rectCoords ) );
	return S_OK;
}