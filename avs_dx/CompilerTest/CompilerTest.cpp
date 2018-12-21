#include "stdafx.h"
#include "Effects.hpp"

#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { __debugbreak(); logError( __hr, #hr ); return __hr; } }

HRESULT test1()
{
	SymbolTable stState;
	Tree tree{ stState };

	const char* e = "if(equal(bb%beatdiv,0),30+rand(30),n)";
	CHECK( tree.parse( e ) );
	return S_OK;
}

HRESULT test2()
{
	auto preset = SuperScope::presets[ 12 ];

	SymbolTable stState;
	Tree tree{ stState };

	Assignments ass;
	CHECK( parseAssignments( preset.init, ass ) );

	for( auto& a : ass )
		CHECK( tree.appendAssignment( a.first, a.second ) );

	CHECK( parseAssignments( preset.frame, ass ) );
	for( auto& a : ass )
		CHECK( tree.appendAssignment( a.first, a.second ) );

	CHECK( parseAssignments( preset.beat, ass ) );
	for( auto& a : ass )
		CHECK( tree.appendAssignment( a.first, a.second ) );

	CHECK( tree.deduceTypes() );

	return S_OK;
}

int main()
{
	test1();
	test2();
	return 0;
}