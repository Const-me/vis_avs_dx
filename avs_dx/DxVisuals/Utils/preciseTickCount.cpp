#include "stdafx.h"
#include "preciseTickCount.h"
#include <mfapi.h>
#pragma comment( lib, "Mfplat.lib" )

namespace
{
	inline uint64_t queryPerformanceCounter()
	{
		uint64_t res;
		QueryPerformanceCounter( (LARGE_INTEGER*)&res );
		return res;
	}

	inline uint64_t queryPerformanceFrequency()
	{
		uint64_t res;
		QueryPerformanceFrequency( (LARGE_INTEGER*)&res );
		return res;
	}

	// BTW, the only reason for this code is Win7 support: on Win8+ there's GetSystemTimePreciseAsFileTime API for that.
	class PreciseTickCount
	{
		const uint32_t msStart;
		const __int64 qpcStart;
		const __int64 qpcFrequency;

	public:
		PreciseTickCount() :
			msStart( GetTickCount() ),
			qpcStart( (__int64)queryPerformanceCounter() ),
			qpcFrequency( (__int64)queryPerformanceFrequency() )
		{ }

		uint32_t get() const
		{
			const __int64 q = queryPerformanceCounter() - qpcStart;
			const __int64 ms = MFllMulDiv( q, 1000, qpcFrequency / 2, qpcFrequency );
			return msStart + (uint32_t)ms;
		}
	};

	const PreciseTickCount s_ptc;
}

uint32_t getPreciseTickCount()
{
	return s_ptc.get();
}