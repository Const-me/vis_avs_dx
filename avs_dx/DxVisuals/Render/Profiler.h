#pragma once
// http://www.reedbeta.com/blog/gpu-profiling-101/

#include <../InteropLib/profilerApi.h>

constexpr uint8_t profilerBuffersCount = 2;

class EffectBase;

class EffectProfiler
{
	friend class Profiler;
	std::array<CComPtr<ID3D11Query>, profilerBuffersCount> m_queries;
	const char* const m_name;

	HRESULT create();

public:
	EffectProfiler( const char* name );
	EffectProfiler( EffectBase* fx );
	~EffectProfiler();

	void mark();
};

class Profiler
{
	class FrameData
	{
		CComPtr<ID3D11Query> disjoint;
		CComPtr<ID3D11Query> begin;
		CComPtr<ID3D11Query> end;
		bool haveMeasures = false;

		struct sEntry
		{
			EffectProfiler* pfx;
			uint32_t level;
		};
		std::vector<sEntry> effects;

	public:

		HRESULT create();

		void frameStart();
		void mark( uint8_t current, uint32_t level, EffectProfiler* fx );
		void frameEnd();

		HRESULT report( uint32_t frame, std::vector<sProfilerEntry> &result, uint8_t buffer );

		void removeEffect( EffectProfiler* pfx );
	};

	std::array<FrameData, profilerBuffersCount> m_frames;
	uint8_t m_buffer = 0;
	uint32_t m_frame = 0;
	uint32_t m_level = 0;

	std::vector<sProfilerEntry> m_result;

	void levelInc()
	{
		m_level++;
	}
	void levelDec()
	{
		assert( m_level > 0 );
		m_level--;
	}

	friend struct ProfilerLevel;

public:
	Profiler();

	void frameStart();
	void mark( EffectProfiler* fx );
	void frameEnd();

	void removeEffect( EffectProfiler* pfx );
};

Profiler& gpuProfiler();

struct ProfilerLevel
{
	ProfilerLevel()
	{
		gpuProfiler().levelInc();
	}
	~ProfilerLevel()
	{
		gpuProfiler().levelDec();
	}
};