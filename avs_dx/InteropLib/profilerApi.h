#pragma once

bool isProfilerEnabled();

bool isProfilerOpen();

void profilerOpen();

void profilerClose();

struct sProfilerEntry
{
	// Pointer that uniquely identifies the effect instance that produced the measure
	const void* measureInstance;

	// Indent level, 0 for top level, increases by 1 inside each list.
	uint32_t level;

	// Name of the measure. Usually it's the name of the effect.
	const char* measure;

	// Timestamp in milliseconds since the previous entry. Levels aren't aggregated, it's all sequential.
	float milliseconds;
};

// Update the GUI. This destroys data in the entries argument: for performance reasons, this function swaps vectors instead of copying them.
bool updateProfilerGui( uint32_t frame, vector<sProfilerEntry>& entries );