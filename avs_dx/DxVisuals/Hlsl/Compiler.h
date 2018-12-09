#pragma once
#include "../Render/Stages.h"
#include "Defines.hpp"

namespace Hlsl
{
	HRESULT compile( eStages what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines = nullptr );
}