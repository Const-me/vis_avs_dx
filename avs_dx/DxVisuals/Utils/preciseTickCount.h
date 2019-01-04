#pragma once

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

uint32_t getPreciseTickCount();