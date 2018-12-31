#pragma once
#include <vector>

bool isProfilerOpen();

void profilerOpen();

void profilerClose();

struct sProfilerEntry
{
	const char* measure;
	float milliseconds;
};

bool updateProfilerGui( uint32_t frame, std::vector<sProfilerEntry>& entries );