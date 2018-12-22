#pragma once
#include "FunctionsTable.h"
#include "VariablesTable.h"

namespace Expressions
{
	struct SymbolTable
	{
		FunctionsTable functions;
		VariablesTable vars;

		SymbolTable() = default;
		SymbolTable( const Prototype& proto ) :
			vars( proto ) { }
	};
}