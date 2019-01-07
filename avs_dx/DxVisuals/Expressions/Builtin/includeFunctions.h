#pragma once
#include "../varEnums.h"

namespace Expressions
{
	struct ShaderFunc
	{
		eVarType returnType;
		const char* hlsl;
	};

	// Try to lookup a function from *.hlsl in this directory, returns nullptr if not found.
	const ShaderFunc* lookupShaderFunc( const CStringA& name );
}