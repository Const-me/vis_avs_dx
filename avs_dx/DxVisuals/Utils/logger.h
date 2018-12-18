#pragma once
#define DXERR_ATL_STRING
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

inline void logMessageV( eLogLevel lvl, const char* pszFormat, va_list args )
{
	CStringA str;
	str.FormatV( pszFormat, args );
	logMessage( lvl, str );
}

#define LOG_MESSAGE_FORMAT( lvl ) va_list args; va_start( args, pszFormat ); logMessageV( lvl, pszFormat, args ); va_end( args );

inline void logError( const char* pszFormat, ... )
{
	LOG_MESSAGE_FORMAT( eLogLevel::Error );
}
inline void logWarning( const char* pszFormat, ... )
{
	LOG_MESSAGE_FORMAT( eLogLevel::Warning );
}
inline void logInfo( const char* pszFormat, ... )
{
	LOG_MESSAGE_FORMAT( eLogLevel::Info );
}
inline void logDebug( const char* pszFormat, ... )
{
	LOG_MESSAGE_FORMAT( eLogLevel::Debug );
}

#undef LOG_MESSAGE_FORMAT