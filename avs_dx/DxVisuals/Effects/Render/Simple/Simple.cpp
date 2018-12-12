#include "stdafx.h"
#include "Simple.h"
using namespace Hlsl::Render::Simple;

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

const StateShaderTemplate& SimpleBase::StateData::shaderTemplate()
{
	return SimpleState();
}

HRESULT SimpleBase::StateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

HRESULT SimpleBase::StateData::defines( StateMacroValues& values ) const
{
	values.add( "num_colors", (int)colors.size() );
	values.uintConstants( "COLOR_VALUES", colors );
	return S_OK;
}

HRESULT Simple::buildState( EffectStateShader& ess )
{
	/* ess.shaderTemplate = SimpleState();
	ess.stateSize = 4;
	.
	setStateOffset( stateBufferOffset );
	thisSize = 4;

	StateMacroValues values{ stateBufferOffset };
	const int numColors = this->avs->num_colors;
	ess.values.add( "num_colors", numColors );
	ess.values.uintConstants( "COLOR_VALUES", uintConstants( this->avs->colors, numColors ) );

	CStringA src = replaceMacros( SimpleStateCS().hlsl, values );

	CStringA main;
	CHECK( splitStateGs( src, main, globals ) );
	hlsl = main;
	return S_OK; */
	return E_NOTIMPL;
}