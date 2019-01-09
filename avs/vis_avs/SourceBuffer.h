#pragma once
#include <EASTL/array.h>

class SourceBuffer
{
	CComAutoCriticalSection m_cs;

	eastl::array<uint8_t, 0x100> g_logtab;

	uint8_t g_visdata[ 2 ][ 2 ][ 576 ];
	bool g_visdata_pstat = true;
	bool g_is_beat;

	int beat_peak1 = 0, beat_peak2 = 0, beat_cnt = 0, beat_peak1_peak = 0;

public:

	void buildLogTable();

	int update( struct winampVisModule *this_mod );

	void copy( char vis_data[ 2 ][ 2 ][ 576 ], bool& beat );
};