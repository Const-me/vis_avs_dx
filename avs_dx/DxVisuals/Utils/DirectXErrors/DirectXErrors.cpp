#include "stdafx.h"
#include "DirectXErrors.h"

// Undefine some of these for D3D-related technologies not used by your app. This will save some binary size. D3D 10, D3D 11, and DXGI are always included.
// #define USE_DIRECT2D
// #define USE_WIC
// #define USE_MF

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#include <ddraw.h>
#include <d3d9.h>
#include <Mmreg.h>
#include <dsound.h>

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>
#include <dinputd.h>
#endif

#include <d3d10_1.h>
#include <d3d11_1.h>

#if !defined(WINAPI_FAMILY) || WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
#include <wincodec.h>
#include <d2derr.h>
#include <dwrite.h>
#endif
#include <Mferror.h>

#include "codes.h"

const char* getDxErrorDescriptionA( HRESULT hr )
{
#define CHK_ERR( code, description )      case code: return description;
#define CHK_ERRA( code )                  case code: return #code;
	switch( hr )
	{
#include "includeData.inl"
	}
#undef CHK_ERRA
#undef CHK_ERR
	return nullptr;
}