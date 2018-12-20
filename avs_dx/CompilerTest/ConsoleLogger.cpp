#include "stdafx.h"
#include "../DxVisuals/Utils/logger.h"

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
}

void logMessage( eLogLevel lvl, const CStringA& msg )
{
	static const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	SetConsoleTextAttribute( consoleHandle, textAttributes( lvl ) );
	printf( "%s\n", msg.operator const char*( ) );
	SetConsoleTextAttribute( consoleHandle, defaultAttributes );
}