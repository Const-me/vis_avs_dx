#pragma once
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>

#include <atlstr.h>
#include <atlcoll.h>
#include <atltypes.h>

#include <vector>
#include <array>
#include <algorithm>

#include <d3d11.h>
#include <SimpleMath.h>
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

#include "../DxVisuals/Utils/logger.h"
#include "../DxVisuals/Expressions/Prototype.h"
#include "../DxVisuals/Expressions/Tree/Tree.h"
#include "../DxVisuals/Expressions/Compiler.h"
#include "../DxVisuals/Expressions/utils.hpp"

using namespace Expressions;