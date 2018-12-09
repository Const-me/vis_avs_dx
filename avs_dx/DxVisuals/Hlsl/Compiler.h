#pragma once
#include "Target.h"
#include "Includes.hpp"
#include "Defines.hpp"

namespace Hlsl
{
	HRESULT compile( eTarget what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines = nullptr, Includes* pIncludes = nullptr );
}