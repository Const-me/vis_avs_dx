#pragma once
#include <Render/Stage.h>
#include "../Defines.h"

namespace Hlsl
{
	const char* targetName( eStage t );

	HRESULT compile( eStage stage, const CStringA& hlsl, const char* name, const CAtlMap<CStringA, CStringA>& inc, const Defines& def, vector<uint8_t>& dxbc, CStringA* pErrorMessages = nullptr );
}