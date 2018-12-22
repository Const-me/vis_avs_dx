#pragma once

namespace Expressions
{
	class Tree;
	HRESULT parseAssignments( const CStringA& preprocessed, Tree& tree );
}