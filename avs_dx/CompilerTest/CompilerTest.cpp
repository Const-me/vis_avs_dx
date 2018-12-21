#include "stdafx.h"
#include "Effects.hpp"

#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { __debugbreak(); logError( __hr, #hr ); return __hr; } }

HRESULT test1()
{
	auto preset = SuperScope::presets[ 12 ];

	SymbolTable stState;
	Tree tree;

	// const char* e = "if(equal(bb%beatdiv,0),f,c)";
	// CHECK( tree.parse( stState, e ) );

	Assignments ass;
	CHECK( parseAssignments( preset.init, ass ) );

	for( auto& a : ass )
		CHECK( tree.appendAssignment( stState, a.first, a.second ) );

	CHECK( parseAssignments( preset.frame, ass ) );
	for( auto& a : ass )
		CHECK( tree.appendAssignment( stState, a.first, a.second ) );

	CHECK( parseAssignments( preset.beat, ass ) );
	for( auto& a : ass )
		CHECK( tree.appendAssignment( stState, a.first, a.second ) );

	return S_OK;
}

int main()
{
	test1();
}