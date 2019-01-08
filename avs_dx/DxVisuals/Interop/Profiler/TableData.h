#pragma once
#include "../profilerApi.h"
#include <EASTL/segmented_vector.h>
#include <atlfile.h>

class TableData
{
	// Effect IDs are 1-based indices in this vector.
	vector<const char*> names;

	static inline uint64_t combine( uint32_t id, uint32_t sn )
	{
		return ( (uint64_t)id ) << 32 | sn;
	}

	struct Effect
	{
		// Integer ID to make subsequent processing steps faster.
		uint32_t id;
		// For each frame, update resets it to 0.
		uint32_t currentIndex;
		// max.count of instances in a single frame, so far.
		uint32_t totalIndices;

		uint64_t columnKey() const { return combine( id, currentIndex ); }
	};
	// Key: sProfilerEntry::measure
	CAtlMap<CStringA, Effect> mapEffects;

	// Key: sEffect::id << 32 | sEffect::currentIndex, Value is column order.
	CAtlMap<uint64_t, uint32_t> mapColumns;

	// Writing that table to HDD would be quite hard, need RDBMS or equivalent.
	// Fortunately, the bandwidth is not that high, maybe ~100mb/hour. When handled with care, that's reasonable amount to keep in RAM.
	struct Entry
	{
		uint32_t frame;
		uint32_t id;
		uint32_t index;
		uint64_t columnKey() const { return combine( id, index ); }
		float milliseconds;
	};
	eastl::segmented_vector<Entry, 0x100000> measures;	// 12MB RAM per segment

	void insertColumnAfter( uint64_t newCol, uint64_t prevCol );

	CAtlFile m_file;

	void clear();

	void reportError( HRESULT hr, const char* what, HWND wnd );

	static HRESULT write( CAtlFile& file, CStringA& buffer );
	static HRESULT appendRow( CAtlFile& file, uint32_t frame, const vector<float>& rows, CStringA& buffer );

public:

	TableData();

	bool start( HWND wnd );

	void add( uint32_t frame, const vector<sProfilerEntry>& entries );

	bool stop( HWND wnd );

	bool writing() const
	{
		return m_file;
	}
};