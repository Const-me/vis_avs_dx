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
	// Background stuff
	static constexpr uint16_t bufferSize = 128;

	struct Entry
	{
		eLogLevel level;
		CStringA message;

		void print( HANDLE hConsole ) const
		{
			SetConsoleTextAttribute( hConsole, textAttributes( level ) );
			WriteConsoleA( hConsole, cstr( message ), message.GetLength(), nullptr, nullptr );
			WriteConsoleA( hConsole, "\r\n", 2, nullptr, nullptr );
		}
	};

	std::deque<Entry> m_entries;
	CComAutoCriticalSection m_cs;

	// GUI stuff
	CHandle m_output;

	void windowClosed()
	{
		CSLock __lock( m_cs );
		m_output.Close();
	}

	static LRESULT __stdcall windowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
	{
		if( uMsg == WM_DESTROY )
		{
			logger().windowClosed();
			RemoveWindowSubclass( hWnd, &windowProc, 0 );
		}
		return DefSubclassProc( hWnd, uMsg, wParam, lParam );
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

		HWND hWnd = GetConsoleWindow();
		if( !hWnd )
			return getLastHr();
		SetWindowSubclass( hWnd, &windowProc, 0, 0 );

		for( const auto& e : m_entries )
			e.print( m_output );

		return S_OK;
	}

public:

	void add( eLogLevel lvl, const CStringA& msg )
	{
		CSLock __lock( m_cs );

		// Add to the local queue
		while( m_entries.size() >= bufferSize )
			m_entries.pop_front();
		m_entries.emplace_back( Entry{ lvl, msg } );
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