#pragma once
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>

#include <atlstr.h>
#include <atlcoll.h>
#include <atltypes.h>

#include <algorithm>
#define EASTL_USER_DEFINED_ALLOCATOR
#include <EASTL/vector.h>
#include <EASTL/array.h>
using eastl::vector;
using eastl::array;

#include <d3d11.h>
#include "../DxVisuals/Utils/DXTK/SimpleMath.h"
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

#include "../DxVisuals/Utils/logger.h"
#include "../DxVisuals/Expressions/Prototype.h"
#include "../DxVisuals/Expressions/Tree/Tree.h"
#include "../DxVisuals/Expressions/Compiler.h"
#include "../DxVisuals/Expressions/utils.hpp"

using namespace Expressions;

inline const char* cstr( const CStringA &str ) { return str; }