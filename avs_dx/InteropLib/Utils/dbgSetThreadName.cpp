#include "stdafx.h"
#include "dbgSetThreadName.h"

bool dbgSetThreadName( const char* threadName )
{
	if( !IsDebuggerPresent() )
		return false;

	// https://stackoverflow.com/a/10364541/126995

	const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		const char* szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	}
	THREADNAME_INFO;
#pragma pack(pop)

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = GetCurrentThreadId();
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return true;
	}
	return false;
}