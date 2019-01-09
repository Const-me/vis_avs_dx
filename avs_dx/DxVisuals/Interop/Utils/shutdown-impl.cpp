#include "stdafx.h"
#include "shutdown.h"
#include "../ConsoleLogger.h"
#include "../profilerApi.h"
#include "../avsUtils.h"

namespace
{

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
		CHandle m_threadShutDown;

	public:

		bool check()
		{
			CSLock __lock( m_cs );
			switch( m_state )
			{
			case eState::Running:
				return false;
			case eState::WaitingForQuit:
				return true;
			case eState::None:
				m_state = eState::Running;
				return false;
			case eState::WaitingForThread:
				onRenderThreadShuttingDown( true );
				SetEvent( m_threadShutDown );
				m_state = eState::WaitingForQuit;
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
					m_threadShutDown.Attach( CreateEvent( nullptr, TRUE, FALSE, nullptr ) );
					if( !m_threadShutDown )
						return getLastHr();
					m_state = eState::WaitingForThread;
					break;
				default:
					__debugbreak();
					return E_UNEXPECTED;
				}
			}
			logShutdown( "Shutdown::shutdown waiting" );
			const HRESULT hr = msgWaitForSingleObject( m_threadShutDown, 500 );
			logShutdown( CStringA{ "Shutdown::shutdown result: " } +formatDxMessageA( hr ) );
			profilerClose();
			closeDebugConsole();
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