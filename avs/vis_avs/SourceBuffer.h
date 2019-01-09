#pragma once
#include <EASTL/array.h>

class SourceBuffer
{
	alignas( 16 ) eastl::array<uint16_t, 0x100> g_logtab;
	alignas( 16 ) uint16_t g_visdata[ 2 ][ 2 ][ 576 ];

	CComAutoCriticalSection m_cs;
	bool g_visdata_pstat = true;
	bool g_is_beat;

	int beat_peak1 = 0, beat_peak2 = 0, beat_cnt = 0, beat_peak1_peak = 0;

public:

	void buildLogTable();

	int update( struct winampVisModule *this_mod );

	void copy( uint16_t vis_data[ 2 ][ 2 ][ 576 ], bool& beat );
};