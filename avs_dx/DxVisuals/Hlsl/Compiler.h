#pragma once
#include "../Render/Stage.h"
#include "Defines.hpp"

namespace Hlsl
{
	HRESULT compile( eStage what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines = nullptr );
}