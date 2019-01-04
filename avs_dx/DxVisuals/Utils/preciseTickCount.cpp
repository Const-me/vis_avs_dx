#include "stdafx.h"
#include "preciseTickCount.h"
#include "PerfMeasure.h"
#include <mfapi.h>
#pragma comment( lib, "Mfplat.lib" )

namespace
{
	// BTW, the only reason for this code is Win7 support: on Win8+ there's GetSystemTimePreciseAsFileTime API for that.
	class PreciseTickCount
	{
		const uint32_t msStart;
		const __int64 qpcStart;

	public:
		const __int64 qpcFrequency;

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

const double PerfMeasure::mulToMs = 1000.0 / s_ptc.qpcFrequency;

PerfMeasure::PerfMeasure( const char* w ) :
	started( queryPerformanceCounter() ),
	what( w )
{ }

PerfMeasure::~PerfMeasure()
{
	const double ms = ( queryPerformanceCounter() - started ) * mulToMs;
	logDebug( "PerfMeasure %s: %.4f ms", what, ms );
}