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

HRESULT DynamicMovement::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	CHECK( m_sampler.update( avs->subpixel, avs->wrap ) );
	BIND_PS_SAMPLER( 1, m_sampler );

	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	BoundPsResource psRead;
	if( avs->blend )
	{
		CHECK( rt.blendToNext( psReadSlot, psRead ) );
		omCustomBlend( 0.5f );
	}
	else
	{
		CHECK( rt.writeToNext( psReadSlot, psRead, false ) );
		omBlend( eBlend::None );
	}
	CHECK( m_mesh.draw( avs->rectcoords ) );
	return S_OK;
}