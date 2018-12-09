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

namespace
{
	// The built-in HRESULT_FROM_WIN32 is an inline function without constexpr in Win8.1 SDK. Need to workaround.
	constexpr HRESULT hresultFromWin32( unsigned long x )
	{
		return (HRESULT)( x ) <= 0 ? (HRESULT)( x ) : (HRESULT)( ( ( x ) & 0x0000FFFF ) | ( FACILITY_WIN32 << 16 ) | 0x80000000 );
	}
}

const char* getDxErrorStringA( HRESULT hr )
{
#define CHK_ERR( code, description )      case code: return #code;
#define CHK_ERRA( code )                  case code: return #code;
#define CHK_ERR_WIN32A( code )            case hresultFromWin32( code ) : return #code;
	switch( hr )
	{
#include "data/win32.inl"
#include "includeData.inl"
	}
#undef CHK_ERR_WIN32A
#undef CHK_ERRA
#undef CHK_ERR
	return nullptr;
}

const wchar_t* getDxErrorStringW( HRESULT hr )
{
#define CHK_ERR( code, description )      case code: return L#code;
#define CHK_ERRA( code )                  case code: return L#code;
#define CHK_ERR_WIN32A( code )            case hresultFromWin32( code ) : return L#code;
	switch( hr )
	{
#include "data/win32.inl"
#include "includeData.inl"
	}
#undef CHK_ERR_WIN32A
#undef CHK_ERRA
#undef CHK_ERR
	return nullptr;
}

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

const wchar_t* getDxErrorDescriptionW( HRESULT hr )
{
#define CHK_ERR( code, description )      case code: return L##description;
#define CHK_ERRA( code )                  case code: return L#code;
	switch( hr )
	{
#include "includeData.inl"
	}
#undef CHK_ERRA
#undef CHK_ERR
	return nullptr;
}