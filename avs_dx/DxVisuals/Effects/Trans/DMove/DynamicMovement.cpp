#include "stdafx.h"
#include "DynamicMovement.h"

IMPLEMENT_EFFECT( DynamicMovement, C_DMoveClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addConstantInput( "b", eVarType::u32 );	// IS_BEAT
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
	def.set( "b", "IS_BEAT" );
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

HRESULT DynamicMovement::render( RenderTargets& rt )
{
	if( !m_mesh )
	{
		const CSize rtSize = getRenderSize();
		// TODO: use config values
		const CSize gridSize = GridMesh::pickSize( rtSize, 32 );
		CHECK( m_mesh.create( gridSize ) );
	}
	// TODO: drop mesh if resized

	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	CHECK( rt.blendToNext( psReadSlot ) );

	renderer.bindShaders();
	CHECK( m_mesh.draw() );

	bindResource<eStage::Pixel>( psReadSlot );
	return S_OK;
}