#pragma once
#include <EASTL/array.h>

// System RAM buffer with the source data for the visualization. This class is thread safe.
class SourceBuffer
{
	alignas( 16 ) eastl::array<uint16_t, 0x100> g_logtab;
	alignas( 16 ) uint16_t g_visdata[ 2 ][ 2 ][ 576 ];

	CComAutoCriticalSection m_cs;
	bool g_visdata_pstat = true;
	bool g_is_beat;

	int beat_peak1 = 0, beat_peak2 = 0, beat_cnt = 0, beat_peak1_peak = 0;

public:

	// Build a pre-calculated logarithm table to transform spectrum data.
	// Possible to eliminate with some SSE trickery, but it's not worth it, the current implementation is quite fast already, the table is just 512 bytes i.e. fits in L1 data cache just fine.
	void buildLogTable();

	// Update from the new data provided by winamp.exe
	int update( struct winampVisModule *this_mod );

	// Copy the most current data from this into the supplied buffer.
	// Beat detection flag is reset by this call, i.e. if AVS renders more often than Winamp updates the data, only the first copy will receive the beat flag.
	void copy( uint16_t vis_data[ 2 ][ 2 ][ 576 ], bool& beat );
};