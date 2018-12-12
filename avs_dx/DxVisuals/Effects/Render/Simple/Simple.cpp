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

HRESULT SimpleBase::StateData::defines( MacroValues& vals ) const
{
	const int num_colors = (int)colors.size();
	values.add( "num_colors", num_colors );
	values.add( "COLOR_VALUES", uintConstants( colors.data(), num_colors ) );
	return S_OK;
}

HRESULT Simple::buildState( int stateBufferOffset, int& thisSize, CStringA& hlsl, bool& useBeat, CAtlMap<CStringA, int>& globals )
{
	setStateOffset( stateBufferOffset );
	thisSize = 4;

	MacroValues values{ stateBufferOffset };
	const int numColors = this->avs->num_colors;
	values.add( "num_colors", numColors );
	values.add( "COLOR_VALUES", uintConstants( this->avs->colors, numColors ) );

	CStringA src = replaceMacros( SimpleStateCS().hlsl, values );

	CStringA main;
	CHECK( splitStateGs( src, main, globals ) );
	hlsl = main;
	return S_OK;
}