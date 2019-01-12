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

#include <atlbase.h>
using CSLock = CComCritSecLock<CComAutoCriticalSection>;
extern CComAutoCriticalSection renderLock;

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

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

#define EASTL_USER_DEFINED_ALLOCATOR
#include <EASTL/vector.h>
using eastl::vector;

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlstr.h>

inline HRESULT getLastHr() { return HRESULT_FROM_WIN32( GetLastError() ); }

#include "r_defs.h"

#include <Threads/threadsApi.h>

const CStringA& getWinampIniPath();