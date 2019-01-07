#include "stdafx.h"
#include "avsUtils.h"

CStringA getPrivateProfileString( const char* lpAppName, const char* lpKeyName, const char* lpFileName )
{
	CStringA res;
	DWORD cc = GetPrivateProfileStringA( lpAppName, lpKeyName, "", res.GetBufferSetLength( MAX_PATH ), MAX_PATH, lpFileName );
	res.ReleaseBufferSetLength( cc );
	return res;
}