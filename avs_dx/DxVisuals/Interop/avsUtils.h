#pragma once
#include "drawDx.h"
#include "../DxVisuals/Utils/logger.h"

CStringA getPrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpFileName );

// Same as WaitForSingleObject but processes windows messages while it waits.
HRESULT msgWaitForSingleObject( HANDLE h, DWORD ms );

constexpr HRESULT E_TIMEOUT = HRESULT_FROM_WIN32( ERROR_TIMEOUT );