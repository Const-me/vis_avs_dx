#pragma once

class PerfMeasure
{
	static const double mulToMs;
	const uint64_t started;
	const char* const what;

public:
	PerfMeasure( const char* w );
	~PerfMeasure();
};