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


ColorModifierStructs::PsData::PsData() :
	m_template{ *ColorModifierPS::shaderTemplate() }
{
	m_hlsl = m_template.hlsl;
}

HRESULT ColorModifier::updateParameters( Binder& binder )
{
	const HRESULT hr = tBase::updateParameters( binder );
	if( S_OK == hr )
	{
		// TODO: set new shader code for PS
		// renderer.data<eStage::Pixel>().
	}
	return hr;
}

HRESULT ColorModifier::render( RenderTargets& rt )
{
	const UINT psReadSlot = renderer.data<eStage::Pixel>().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, false ) );

	return E_NOTIMPL;
}