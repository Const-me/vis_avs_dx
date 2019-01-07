#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
// Windows headers
#include <windows.h>
#include <versionhelpers.h>

// Direct3D
#include <d3d11.h>
#include "Utils/DXTK/SimpleMath.h"
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

// ATL
#define _WINDLL
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atltypes.h>
#include <atlcoll.h>
#include <atlstr.h>

// C++
#include <stdint.h>
#include <algorithm>
#include <EASTL/vector.h>
#include <EASTL/array.h>
#include <EASTL/unique_ptr.h>
using eastl::vector;
using eastl::array;
using eastl::unique_ptr;
using eastl::make_unique;
#include <functional>

#include <mfobjects.h>

#include "Interop/device.h"
#include "Utils/logger.h"

#include "Utils/MiscUtils.h"
#include "Utils/BoolHr.hpp"
#include "Resources/bindResource.h"

using CSLock = CComCritSecLock<CComAutoCriticalSection>;

#define GPU_PROFILE 1