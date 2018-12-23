#pragma once

namespace Expressions
{
	class Tree;
	HRESULT parseStatements( const CStringA& preprocessed, Tree& tree );
}