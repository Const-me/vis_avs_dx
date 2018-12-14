#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )

using PsData = ColorModifierStructs::PsData;

HRESULT ColorModifierStructs::PsData::update( const AvsState& ass )
{
	// TODO: update these expressions
	return S_FALSE;
}

HRESULT ColorModifier::render( RenderTargets& rt )
{
	const UINT psReadSlot = data<eStage::Pixel>().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, false ) );

	return E_NOTIMPL;
}