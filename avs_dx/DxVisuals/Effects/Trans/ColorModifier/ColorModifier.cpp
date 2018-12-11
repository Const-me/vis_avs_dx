#include "stdafx.h"
#include "ColorModifier.h"

IMPLEMENT_EFFECT( ColorModifier, C_DColorModClass )

using PsData = ColorModifierStructs::PsData;

const ShaderTemplate& PsData::shaderTemplate()
{
	static const ShaderTemplate res{ "ColorModifierPS", Hlsl::Trans::ColorModifier::ColorModifierPS() };
	return res;
}

bool PsData::update( const AvsState& ass, int stateOffset )
{
	return false;
}

HRESULT PsData::defines( Hlsl::Defines& def ) const
{
	def.set( "SHADER_CODE", hlsl );
	return S_OK;
}