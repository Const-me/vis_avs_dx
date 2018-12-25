#pragma once

class EventSlim
{
	CComAutoCriticalSection m_cs;
	CONDITION_VARIABLE m_cv;
	bool m_state = false;

public:
	EventSlim();

	void set();
	void reset();
	HRESULT wait();
};