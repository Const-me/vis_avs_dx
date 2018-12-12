#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
// Windows headers
#include <windows.h>

// Direct3D
#include <d3d11.h>
#include <SimpleMath.h>
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

// ATL
#include <atltypes.h>
#include <atlcoll.h>
#include <atlstr.h>
// #include <atlcom.h>

// C++
#include <stdint.h>
#include <memory>
#include <algorithm>
#include <array>
#include <vector>
#include <functional>

extern CComPtr<ID3D11Device> device;
extern CComPtr<ID3D11DeviceContext> context;

#define DXERR_ATL_STRING
#include "Utils/logger.h"

#include "Utils/MiscUtils.h"
#include "Resources/bindResource.h"

using CSLock = CComCritSecLock<CComAutoCriticalSection>;