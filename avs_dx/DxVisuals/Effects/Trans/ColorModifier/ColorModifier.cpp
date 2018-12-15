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
			addInput( "beat", eVarType::u32 );	// IS_BEAT
			addOutput( "red" );
			addOutput( "green" );
			addOutput( "blue" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

HRESULT ColorModifierStructs::StateData::defines( Hlsl::Defines& def ) const
{
	def.set( "beat", "IS_BEAT" );
	return S_OK;
}

ColorModifierStructs::StateData::StateData( AvsState& ass ) :
	Compiler( "ColorModifier", prototype() )
{ }

HRESULT ColorModifier::render( RenderTargets& rt )
{
	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, false ) );

	return E_NOTIMPL;
}