#include "stdafx.h"
#include <deque>

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

class Logger;
Logger& logger();

class Logger
{
	// Background stuff: accumulate messages in a small buffer, in case user will want to see them in the console.
	// Ideally, we should accumulate them in a more efficient data structure, maybe a circular buffer.
	// However, we don't have that many messages per second, this simple solution that uses std::deque is probably good enough for the job.
	static constexpr uint16_t bufferSize = 64;

	struct Entry
	{
		eLogLevel level;
		CStringA message;

		HRESULT print( HANDLE hConsole ) const
		{
			if( !SetConsoleTextAttribute( hConsole, textAttributes( level ) ) )
				return getLastHr();
			if( !WriteConsoleA( hConsole, cstr( message ), message.GetLength(), nullptr, nullptr ) )
				return getLastHr();
			if( !WriteConsoleA( hConsole, "\r\n", 2, nullptr, nullptr ) )
				return getLastHr();
			return S_OK;
		}
	};

	std::deque<Entry> m_entries;
	CComAutoCriticalSection m_cs;

	// GUI stuff
	CHandle m_output;

	void windowClosed()
	{
		CSLock __lock( m_cs );
		if( FreeConsole() )
		{
			// Apparently, FreeConsole already closes that handle: https://stackoverflow.com/q/12676312/126995
			m_output.Detach();
		}
		m_output.Close();
	}

	static BOOL __stdcall consoleHandlerRoutine( DWORD dwCtrlType )
	{
		switch( dwCtrlType )
		{
		case CTRL_CLOSE_EVENT:
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
			logger().windowClosed();
			logDebug( "Debug console closed" );
			return TRUE;
		}
		return TRUE;
	}

	HRESULT showConsole()
	{
		if( !AllocConsole() )
			return getLastHr();
		m_output.Close();
		m_output.Attach( GetStdHandle( STD_OUTPUT_HANDLE ) );
		if( !m_output )
			return getLastHr();
		constexpr UINT cp = 1252;	// ANSI Latin 1; Western European (Windows)
		if( IsValidCodePage( cp ) )
			SetConsoleOutputCP( cp );

		SetConsoleTitle( L"Winamp AVS DX Debug Console" );

		SetConsoleCtrlHandler( &consoleHandlerRoutine, TRUE );

		// Disable close command in the sys.menu of the new console, otherwise the whole Winamp process will quit: https://stackoverflow.com/a/12015131/126995
		HWND hwnd = ::GetConsoleWindow();
		if( hwnd != nullptr )
		{
			HMENU hMenu = ::GetSystemMenu( hwnd, FALSE );
			if( hMenu != NULL )
				DeleteMenu( hMenu, SC_CLOSE, MF_BYCOMMAND );
		}

		// Print old log entries
		for( const auto& e : m_entries )
			CHECK( e.print( m_output ) );

		const CStringA msg = "Press Control+C or Control+Break to close this window\r\n";
		if( !SetConsoleTextAttribute( m_output, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY ) )
			return getLastHr();
		if( !WriteConsoleA( m_output, cstr( msg ), msg.GetLength(), nullptr, nullptr ) )
			return getLastHr();

		return S_OK;
	}

public:

	void add( eLogLevel lvl, const CStringA& msg )
	{
		CSLock __lock( m_cs );

		// Add to the buffer
		while( m_entries.size() >= bufferSize )
			m_entries.pop_front();
		m_entries.emplace_back( Entry{ lvl, msg } );

		// If the console window is shown, print there, too.
		if( m_output )
			m_entries.rbegin()->print( m_output );
	}

	void show()
	{
		CSLock __lock( m_cs );

		HWND hWnd = GetConsoleWindow();
		if( nullptr != hWnd )
		{
			ShowWindow( hWnd, SW_RESTORE );
			SetForegroundWindow( hWnd );
			return;
		}

		showConsole();
	}

	void close()
	{
		CSLock __lock( m_cs );
		if( !m_output )
			return;
		windowClosed();
	}
};

Logger& logger()
{
	static Logger s_logger;
	return s_logger;
}

void logMessage( eLogLevel lvl, const CStringA& msg )
{
	logger().add( lvl, msg );
}

void showDebugConsole()
{
	logger().show();
}

void closeDebugConsole()
{
	logger().close();
}