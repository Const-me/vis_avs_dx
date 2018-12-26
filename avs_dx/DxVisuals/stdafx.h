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
#define _WINDLL
#include <atltypes.h>
#include <atlcoll.h>
#include <atlstr.h>

// C++
#include <stdint.h>
#include <memory>
#include <algorithm>
#include <array>
#include <vector>
#include <functional>

#include <mfobjects.h>

extern CComPtr<ID3D11Device> device;
extern CComPtr<ID3D11DeviceContext> context;
extern CComPtr<IMFDXGIDeviceManager> dxgiDeviceManager;

#include "Utils/logger.h"

#include "Utils/MiscUtils.h"
#include "Utils/BoolHr.hpp"
#include "Resources/bindResource.h"

using CSLock = CComCritSecLock<CComAutoCriticalSection>;