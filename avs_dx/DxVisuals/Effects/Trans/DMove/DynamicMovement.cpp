#include "stdafx.h"
#include "DynamicMovement.h"
#include <../InteropLib/interop.h>

IMPLEMENT_EFFECT( DynamicMovement, C_DMoveClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addConstantInput( "w", eVarType::u32 );	// screen width in pixels
			addConstantInput( "h", eVarType::u32 );	// screen height in pixels

			addState( "alpha", 0.5f );

			addFragmentOutput( "d" );
			addFragmentOutput( "r" );
			addFragmentOutput( "x" );
			addFragmentOutput( "y" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

HRESULT DynamicMovementStructs::StateData::defines( Hlsl::Defines& def ) const
{
	def.set( "w", screenSize.cx );
	def.set( "h", screenSize.cy );
	return S_OK;
}

DynamicMovementStructs::StateData::StateData( AvsState& ass ) :
	Compiler( "DynamicMovement", prototype() )
{ }

HRESULT DynamicMovementStructs::StateData::updateInputs( const AvsState& ass )
{
	return updateInput( screenSize, getRenderSize() );
}

HRESULT DynamicMovementStructs::VsData::compiledShader( const std::vector<uint8_t>& dxbc )
{
	return StaticResources::createLayout( dxbc );
}

HRESULT DynamicMovementStructs::VsData::updateAvs( const AvsState &avs )
{
	const CSize rs = getRenderSize();
	Vector2 diag{ (float)rs.cx, (float)rs.cy };
	diag.Normalize();

	BoolHr res;
	res.updateValue( scaleToUniform, diag );
	res.updateValue( rectangularCoords, 0 != avs.rectcoords );
	return res;
}

DynamicMovement::DynamicMovement( AvsState *pState ) :
	tBase( pState )
{ }

void DynamicMovement::onRenderSizeChanged()
{
	m_mesh.destroy();
}

HRESULT DynamicMovement::render( bool isBeat, RenderTargets& rt )
{
	if( !m_mesh )
	{
		const CSize rtSize = getRenderSize();
		// TODO: use config values
		const CSize gridSize = GridMesh::pickSize( rtSize, 32 );
		CHECK( m_mesh.create( gridSize ) );
	}

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	if( avs->wrap )
	{
		if( !m_wrapSampler )
		{
			CD3D11_SAMPLER_DESC sd{ D3D11_DEFAULT };
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			CHECK( device->CreateSamplerState( &sd, &m_wrapSampler ) );
		}
		bindSampler<eStage::Pixel>( 1, m_wrapSampler );
	}
	else
	{
		if( !m_clampSampler )
		{
			CD3D11_SAMPLER_DESC sd{ D3D11_DEFAULT };
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			CHECK( device->CreateSamplerState( &sd, &m_clampSampler ) );
		}
		bindSampler<eStage::Pixel>( 1, m_clampSampler );
	}

	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, true ) );
	omBlend( eBlend::None );
	context->RSSetState( StaticResources::rsDisableCulling );
	CHECK( m_mesh.draw() );

	bindResource<eStage::Pixel>( psReadSlot );
	context->RSSetState( nullptr );
	bindSampler<eStage::Pixel>( 1 );
	return S_OK;
}