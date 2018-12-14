#include "stdafx.h"
#include "DynamicMovement.h"
using namespace Expressions;

IMPLEMENT_EFFECT( DynamicMovement, C_DMoveClass )

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
		}
	};
	static const Expressions::Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

DynamicMovementStructs::StateData::StateData( AvsState& ass ):
	Compiler( "DynamicMovement", prototype() )
{

}

DynamicMovement::DynamicMovement( AvsState *pState ) :
	tBase( pState )
{ }

HRESULT DynamicMovement::render( RenderTargets& rt )
{
	const UINT psReadSlot = data<eStage::Pixel>().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, true ) );

	return E_NOTIMPL; 
}