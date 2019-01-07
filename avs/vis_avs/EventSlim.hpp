#pragma once

class CondVar
{
	CONDITION_VARIABLE m_cv;
	
public:
	static constexpr HRESULT E_TIMEOUT = HRESULT_FROM_WIN32( ERROR_TIMEOUT );

	CondVar()
	{
		InitializeConditionVariable( &m_cv );
	}
	CondVar( const CondVar& ) = delete;
	CondVar( CondVar&& ) = delete;
	void operator=( const CondVar& ) = delete;

	void wakeOne()
	{
		WakeConditionVariable( &m_cv );
	}
	void wakeAll()
	{
		WakeAllConditionVariable( &m_cv );
	}

	template<class TCond>
	HRESULT wait( CRITICAL_SECTION &cs, TCond condition, DWORD ms = INFINITE )
	{
		if( condition() )
			return S_OK;

		DWORD started = GetTickCount();
		while( true )
		{
			if( !SleepConditionVariableCS( &m_cv, &cs, ms ) )
				return getLastHr();

			if( condition() )
				return S_OK;

			if( ms == INFINITE )
				continue;

			const DWORD now = GetTickCount();
			const DWORD elapsed = now - started;
			if( elapsed >= ms )
				return E_TIMEOUT;
			ms -= elapsed;
			started = now;
		}
	}
};

using CSLock = CComCritSecLock<CComAutoCriticalSection>;

struct EventSlim
{
	CComAutoCriticalSection m_cs;

	void set()
	{
		{
			CSLock __lock( m_cs );
			m_signaled = true;
		}
		m_cv.wakeAll();
	}

	void reset()
	{
		CSLock __lock( m_cs );
		m_signaled = false;
	}

	HRESULT wait( DWORD ms = INFINITE )
	{
		CSLock __lock( m_cs );
		return m_cv.wait( m_cs.m_sec, [ this ]() {return m_signaled; }, ms );
	}

private:
	CondVar m_cv;
	bool m_signaled = false;
};