#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
// Windows headers
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <commdlg.h>
#include <process.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <time.h>

#include <algorithm>
// Interop with the AVS code
inline int min( int a, int b )
{
	return std::min( a, b );
}
inline int max( int a, int b )
{
	return std::max( a, b );
}

#include "r_defs.h"

#include <EASTL/vector.h>
using eastl::vector;