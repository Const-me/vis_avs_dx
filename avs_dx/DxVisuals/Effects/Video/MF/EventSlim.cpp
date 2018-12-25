#include "stdafx.h"
#include "EventSlim.h"

EventSlim::EventSlim()
{
	InitializeConditionVariable( &m_cv );
}

void EventSlim::set()
{
	{
		CSLock __lock( m_cs );
		m_state = true;
	}
	WakeConditionVariable( &m_cv );
}

void EventSlim::reset()
{
	CSLock __lock( m_cs );
	m_state = false;
}

HRESULT EventSlim::wait()
{
	CSLock __lock( m_cs );
	while( !m_state )
	{
		if( SleepConditionVariableCS( &m_cv, &m_cs.m_sec, INFINITE ) )
			continue;
		return getLastHr();
	}
	return S_OK;
}