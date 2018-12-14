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
			addInput( "b", eVarType::u32 );	// IS_BEAT
			addInput( "w", eVarType::u32 );	// screen width in pixels
			addInput( "h", eVarType::u32 );	// screen height in pixels

			addState( "alpha", 0.5f );

			addOutput( "d" );
			addOutput( "r" );
			addOutput( "x" );
			addOutput( "y" );
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

DynamicMovementStructs::StateData::StateData( AvsState& ass ):
	Compiler( "DynamicMovement", prototype() )
{ }

HRESULT DynamicMovementStructs::StateData::updateInputs( const AvsState& ass )
{
	HRESULT hr = S_FALSE;
	updateInput( screenSize, getRenderSize(), hr );
	return hr;
}

DynamicMovement::DynamicMovement( AvsState *pState ) :
	tBase( pState )
{ }

HRESULT DynamicMovement::render( RenderTargets& rt )
{
	const UINT psReadSlot = renderer.data<eStage::Pixel>().bindPrevFrame;
	CHECK( rt.blendToNext( psReadSlot ) );

	return E_NOTIMPL; 
}