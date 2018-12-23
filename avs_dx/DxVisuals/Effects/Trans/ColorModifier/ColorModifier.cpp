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
			addConstantInput( "beat", eVarType::u32 );	// IS_BEAT
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

HRESULT ColorModifierStructs::StateData::defines( Hlsl::Defines& def )
{
	def.set( "beat", "IS_BEAT" );
	return S_OK;
}

ColorModifierStructs::StateData::StateData( AvsState& ass ) :
	Compiler( "ColorModifier", prototype() )
{ }

HRESULT ColorModifier::render( RenderTargets& rt )
{
	omDontBlend();
	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, false ) );

	renderer.bindShaders();
	drawFullscreenTriangle( false );

	bindResource<eStage::Pixel>( psReadSlot );
	return S_OK;
}