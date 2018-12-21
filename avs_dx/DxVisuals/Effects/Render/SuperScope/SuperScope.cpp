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
			addConstantInput( "b", eVarType::u32 );	// IS_BEAT
			addConstantInput( "w", eVarType::u32 );	// screen width in pixels
			addConstantInput( "h", eVarType::u32 );	// screen height in pixels

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