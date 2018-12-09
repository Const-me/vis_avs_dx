#include "stdafx.h"
#include "DirectXErrors.h"
#include "formatMessage.hpp"

#ifdef DXERR_ATL_STRING
CStringA formatDxMessageA( HRESULT hr )
{
	// Custom messages
	const char* const description = getDxErrorDescriptionA( hr );
	if( nullptr != description )
		return CStringA{ description };

	// Windows messages
	char *lpMsgBuf;
	const DWORD fmLength = formatMessageA( hr, &lpMsgBuf );
	if( 0 != fmLength )
	{
		CStringA msg{ lpMsgBuf, (int)fmLength };
		LocalFree( lpMsgBuf );
		msg.Trim( "\r\n\t " );
		return msg;
	}

	// Fallback message
	CStringA unknown;
	unknown.Format( "Unknown error code %i (0x%08X)", (int)hr, (int)hr );
	return unknown;
}

CStringW formatDxMessageW( HRESULT hr )
{
	// Custom messages
	const char* const description = getDxErrorDescriptionA( hr );	// Using the "A" version saves binary size
	if( nullptr != description )
		return CStringW{ description };

	// Windows messages
	wchar_t *lpMsgBuf;
	const DWORD fmLength = formatMessageW( hr, &lpMsgBuf );
	if( 0 != fmLength )
	{
		CStringW msg{ lpMsgBuf, (int)fmLength };
		LocalFree( lpMsgBuf );
		msg.Trim( L"\r\n\t " );
		return msg;
	}

	// Fallback message
	CStringW unknown;
	unknown.Format( L"Unknown error code %i (0x%08X)", (int)hr, (int)hr );
	return unknown;
}
#endif