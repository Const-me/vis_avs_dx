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
			addInput( "b", eVarType::u32 );	// IS_BEAT
			addInput( "w", eVarType::u32 );	// screen width in pixels
			addInput( "h", eVarType::u32 );	// screen height in pixels

			addState( "n", 768u );

			addOutput( "x" );
			addOutput( "y" );

			addOutput( "skip" );
			addOutput( "linesize" );

			addOutput( "red" );
			addOutput( "green" );
			addOutput( "blue" );

			// Actually they're inputs; need to rename the API, e.g. addInput -> addConstant, addOutput -> addVar
			addOutput( "i" );
			addOutput( "v" );
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