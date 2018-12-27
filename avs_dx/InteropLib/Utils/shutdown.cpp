#include "stdafx.h"
#include "shutdown.h"
#include "../../DxVisuals/Utils/effectsRegistry.h"

namespace
{
	constexpr HRESULT E_TIMEOUT = HRESULT_FROM_WIN32( ERROR_TIMEOUT );
	constexpr int MSGF_SLEEPMSG = 0x5300;

	HRESULT msgWaitForSingleObject( HANDLE h, DWORD ms )
	{
		// https://blogs.msdn.microsoft.com/oldnewthing/20060126-00/?p=32513/
		while( true )
		{
			const DWORD started = GetTickCount();
			const DWORD res = MsgWaitForMultipleObjectsEx( 1, &h, ms, QS_ALLINPUT, MWMO_INPUTAVAILABLE );
			if( WAIT_OBJECT_0 == res )
				return S_OK;
			if( WAIT_OBJECT_0 + 1 == res )
			{
				MSG msg;
				while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
				{
					if( msg.message == WM_QUIT )
					{
						PostQuitMessage( (int)msg.wParam );
						return S_FALSE; // Abandoned due to WM_QUIT
					}
					if( !CallMsgFilter( &msg, MSGF_SLEEPMSG ) )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
				}
				const DWORD elapsed = GetTickCount() - started;	//< This works OK even when DWORD overflows: https://codereview.stackexchange.com/a/129670/46194
				if( elapsed > ms )
					return E_TIMEOUT;
				ms -= elapsed;
				continue;
			}
			if( WAIT_TIMEOUT == res )
				return E_TIMEOUT;
			HRESULT hr = getLastHr();
			if( FAILED( hr ) )
				return hr;
			return E_UNEXPECTED;	// Unasked IO_COMPLETION or ABANDONED?
		}
	}

	class Shutdown
	{
		CComAutoCriticalSection m_cs;

		enum struct eState : uint8_t
		{
			None,
			Running,
			WaitingForThread,
			WaitingForQuit
		};

		eState m_state = eState::None;
		CHandle m_threadIdle;

	public:

		Shutdown()
		{
			m_threadIdle.Attach( CreateEvent( nullptr, TRUE, FALSE, nullptr ) );
		}

		bool check()
		{
			CSLock __lock( m_cs );
			switch( m_state )
			{
			case eState::Running:
				return false;
			case eState::WaitingForQuit:
				Sleep( 10 );
				return true;
			case eState::None:
				m_state = eState::Running;
				return false;
			case eState::WaitingForThread:
				m_state = eState::WaitingForQuit;
				onRenderThreadShuttingDown();
				SetEvent( m_threadIdle );
				return true;
			}
			__debugbreak();
			return true;
		}

		HRESULT shutdown()
		{
			logShutdown( "Shutdown::shutdown" );
			{
				CSLock __lock( m_cs );
				switch( m_state )
				{
				case eState::None:
					return S_FALSE;
				case eState::Running:
					m_state = eState::WaitingForThread;
					break;
				default:
					__debugbreak();
					return E_UNEXPECTED;
				}
			}
			logShutdown( "Shutdown::shutdown waiting" );
			const HRESULT hr = msgWaitForSingleObject( m_threadIdle, 500 );
			logShutdown( CStringA{ "Shutdown::shutdown result: " } +formatDxMessageA( hr ) );
			return hr;
		}
	};

	static Shutdown g_shutdown;
}

bool checkShutdown()
{
	return g_shutdown.check();
}

HRESULT shutdownThread()
{
	return g_shutdown.shutdown();
}