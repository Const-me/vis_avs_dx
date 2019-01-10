#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "beat" );
			addFragmentOutput( "red" );
			addFragmentOutput( "green" );
			addFragmentOutput( "blue" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

ColorModifierStructs::StateData::StateData( AvsState& ass ) :
	Compiler( "ColorModifier", prototype() )
{ }

HRESULT ColorModifier::createTexture()
{
	if( m_uav )
		return S_FALSE;

	constexpr DXGI_FORMAT fmt = DXGI_FORMAT_R10G10B10A2_UNORM;
	CD3D11_TEXTURE1D_DESC td{ fmt, 1024, 1,1,D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	CHECK( device->CreateTexture1D( &td, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC sd{ D3D11_SRV_DIMENSION_TEXTURE1D, fmt };
	CHECK( device->CreateShaderResourceView( m_texture, &sd, &m_srv ) );

	CD3D11_UNORDERED_ACCESS_VIEW_DESC ud{ D3D11_UAV_DIMENSION_TEXTURE1D, fmt };
	CHECK( device->CreateUnorderedAccessView( m_texture, &ud, &m_uav ) );

	return S_OK;
}

HRESULT ColorModifier::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return false;

	BoolHr hr = createTexture();
	hr.combine( renderer.compute().m_updated );
	hr.combine( avs->m_recompute ? true : false );
	if( hr.failed() )
		return hr;
	if( hr.value() )
	{
		BoundUav bound{ renderer.compute().bindTable, m_uav };
		context->Dispatch( 4, 1, 1 );
	}

	omBlend( eBlend::None );
	BoundPsResource boundFrame;
	CHECK( rt.writeToNext( boundFrame ) );
	BoundPsResource boundTable{ renderer.pixel().bindTable, m_srv };

	drawFullscreenTriangle( false );
	return S_OK;
}