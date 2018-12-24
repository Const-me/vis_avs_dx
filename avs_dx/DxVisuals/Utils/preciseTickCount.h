#pragma once

uint32_t getPreciseTickCount();

class PerfMeasure
{
	static const double mulToMs;
	const uint64_t started;
	const char* const what;

public:
	PerfMeasure( const char* w );
	~PerfMeasure();
};