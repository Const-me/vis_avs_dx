#include "stdafx.h"
#include "Simple.h"
using namespace Hlsl::Render::Simple;

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

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