#pragma once
#include <vector>

bool isProfilerEnabled();

bool isProfilerOpen();

void profilerOpen();

void profilerClose();

struct sProfilerEntry
{
	uint32_t level;
	const char* measure;
	float milliseconds;
};

bool updateProfilerGui( uint32_t frame, std::vector<sProfilerEntry>& entries );