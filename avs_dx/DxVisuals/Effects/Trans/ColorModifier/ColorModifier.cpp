#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )

using PsData = ColorModifierStructs::PsData;

bool PsData::update( const AvsState& ass, int stateOffset )
{
	return false;
}

HRESULT ColorModifier::render( RenderTargets& rt )
{
	return E_NOTIMPL;
}