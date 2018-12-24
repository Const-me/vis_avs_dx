#include "stdafx.h"
#include "SuperScope.h"

using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addConstantInput( "w", eVarType::u32 );	// screen width in pixels
			addConstantInput( "h", eVarType::u32 );	// screen height in pixels
			addConstantInput( "drawingLines", eVarType::u32 );

			addState( "n", 768u );

			addFragmentOutput( "x" );
			addFragmentOutput( "y" );

			addFragmentOutput( "skip" );
			addFragmentOutput( "linesize" );

			addFragmentOutput( "red" );
			addFragmentOutput( "green" );
			addFragmentOutput( "blue" );

			addFragmentInput( "i" );
			addFragmentInput( "v" );

			addIndirectDrawArgs( "drawArgs", uint4{ 0, 1, 0, 0 }, "drawArgs.x = drawingLines ? n + 2 : n;" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

ScopeBase::DynamicStateData::DynamicStateData():
	Expressions::Compiler( "SuperScope", prototype() ) { }