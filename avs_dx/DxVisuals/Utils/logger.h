#pragma once
#include "DirectXErrors/DirectXErrors.h"

enum struct eLogLevel : uint8_t
{
	Error,
	Warning,
	Info,
	Debug
};

extern void logMessage( eLogLevel lvl, const CStringA& msg );

inline void logError( HRESULT hr, const CStringA& what )
{
	const CStringA formatted = formatDxMessageA( hr );
	logMessage( eLogLevel::Error, what + ": " + formatted );
}

inline void logWarning( HRESULT hr, const CStringA& what )
{
	const CStringA formatted = formatDxMessageA( hr );
	logMessage( eLogLevel::Warning, what + ": " + formatted );
}

inline void logInfo( const char* pszFormat, ... )
{
	CStringA str;
	va_list args;
	va_start( args, pszFormat );
	str.FormatV( pszFormat, args );
	va_end( args );
	logMessage( eLogLevel::Info, str );
}

inline void logDebug( const char* pszFormat, ... )
{
	CStringA str;
	va_list args;
	va_start( args, pszFormat );
	str.FormatV( pszFormat, args );
	va_end( args );
	logMessage( eLogLevel::Debug, str );
}