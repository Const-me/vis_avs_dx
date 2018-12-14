#pragma once
#include "eVarType.h"

namespace Expressions
{
	struct ShaderFunc
	{
		eVarType returnType;
		const char* hlsl;
	};

	const ShaderFunc* lookupShaderFunc( const CStringA& name );
}