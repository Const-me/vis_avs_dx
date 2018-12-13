#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )

using PsData = ColorModifierStructs::PsData;

HRESULT ColorModifier::render( RenderTargets& rt )
{
	return E_NOTIMPL;
}