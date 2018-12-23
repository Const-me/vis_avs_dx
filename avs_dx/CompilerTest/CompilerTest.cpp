#include "stdafx.h"
#include "Effects.hpp"
#include "../DxVisuals/Expressions/preprocess.h"
#include "../DxVisuals/Expressions/parse.h"

#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { __debugbreak(); logError( __hr, #hr ); return __hr; } }

HRESULT test0()
{
	CStringA str = R"fffuuu(

fff;
aa=bb;
/****  // */
		cc = dd;
// /* */
12*/3/4

/*
567
//*/

890

//*
567
//*/

)fffuuu";

	preprocess( str );
	return S_OK;
}

HRESULT test1()
{
	SymbolTable stState;
	Tree tree{ stState };

	const char* e =
		// "if(equal(bb%beatdiv,0),30+rand(30),sin(n*$PI))";
		"red=red*3;\r\ngreen=green*2.7;\r\nblue=rand()";

	CStringA str = e;
	preprocess( str );
	CHECK( tree.parse( str ) );
	tree.dbgPrint();
	return S_OK;
}

HRESULT test2()
{
	auto preset = SuperScope::presets[ 12 ];

	const SuperScope::Proto proto;

	Compiler compiler{ "SuperScope", proto };
	CHECK( compiler.update( preset.init, preset.frame, preset.beat, preset.point ) );

	return S_OK;
}

HRESULT test3()
{
	const Move::Proto proto;
	SymbolTable stState{ proto };
	Tree tree{ stState };

	const char* e = "p=gettime(0); v=1-(y+1)*0.5; y=y+0.25*(getosc(v,0.2,0)) + sin(p-1)*0.03;";

	CStringA str = e;
	preprocess( str );

	CHECK( parseAssignments( str, tree ) );
	CHECK( tree.deduceTypes() );
	// tree.transformDoubleFuncs();
	tree.dbgPrint();
	return S_OK;
}

int main()
{
	// test0();
	// test1();
	// test2();
	test3();
	return 0;
}