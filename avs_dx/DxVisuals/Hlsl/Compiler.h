#pragma once
#include "../Render/Stage.h"
#include "Defines.h"

namespace Hlsl
{
	HRESULT compile( eStage what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, const Defines* pDefines = nullptr );
}