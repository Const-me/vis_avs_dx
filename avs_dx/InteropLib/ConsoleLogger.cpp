#include "stdafx.h"

namespace
{
	constexpr uint16_t defaultAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

	inline uint16_t textAttributes( eLogLevel lvl )
	{
		switch( lvl )
		{
		case eLogLevel::Error:
			return FOREGROUND_RED | FOREGROUND_INTENSITY;
		case eLogLevel::Warning:
			return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case eLogLevel::Info:
			return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case eLogLevel::Debug:
			return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		}
		return defaultAttributes;
	}

	HANDLE createConsole()
	{
		AllocConsole();
		// https://stackoverflow.com/a/15547699/126995
		freopen( "CONOUT$", "w", stdout );
		return GetStdHandle( STD_OUTPUT_HANDLE );
	}
}

void logMessage( eLogLevel lvl, const CStringA& msg )
{
	static CComAutoCriticalSection cs;
	CSLock __lock( cs );
	static const HANDLE consoleHandle = createConsole();

	SetConsoleTextAttribute( consoleHandle, textAttributes( lvl ) );
	printf( "%s\n", msg.operator const char*( ) );
	SetConsoleTextAttribute( consoleHandle, defaultAttributes );
}