#pragma once
#include "../Hlsl/parseId.hpp"

namespace Expressions
{
	class Tree;
	HRESULT parseAssignments( const CStringA& preprocessed, Tree& tree );
}