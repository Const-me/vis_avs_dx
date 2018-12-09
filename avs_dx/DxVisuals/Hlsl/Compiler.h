#pragma once
#include "../Render/Target.h"
#include "Defines.hpp"

namespace Hlsl
{
	HRESULT compile( eTarget what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines = nullptr );
}