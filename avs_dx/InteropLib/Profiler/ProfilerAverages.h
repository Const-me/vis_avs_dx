#pragma once
#include "../profilerApi.h"
#include <../DxVisuals/Utils/DynamicArray.hpp>

// Aggregates profiler data across frames, and computes rolling average of the measures.
class ProfilerAverages
{
	static constexpr uint8_t framesCount = 64;

	struct sEntry
	{
		// The unit is microsecond. Using integers here because floats would eventually screw up the fast algorithm of calculating the rolling average due to precision issues.
		DynamicArray<uint32_t, framesCount> data;
		uint32_t total;
		uint8_t next;
		bool present;
	};

	CAtlMap<const void*, sEntry> m_map;

	static float updateEntry( sEntry& e, float current );

	float createEntry( const void* key, float current );

	vector<POSITION> m_drop; // Placing it here instead of a local variable to avoid dynamic allocations in runtime

	vector<float> m_result;

public:

	ProfilerAverages();

	bool update( const vector<sProfilerEntry>& entries );

	float operator[]( size_t i ) const
	{
		return m_result[ i ];
	}
};