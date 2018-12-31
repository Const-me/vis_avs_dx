#pragma once
// http://www.reedbeta.com/blog/gpu-profiling-101/

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
		std::vector<EffectProfiler*> effects;

	public:

		HRESULT create();

		void frameStart();
		void mark( uint8_t current, EffectProfiler* fx );
		void frameEnd();

		HRESULT report( CStringA &message, uint8_t frame );
	};

	std::array<FrameData, profilerBuffersCount> m_frames;
	uint8_t m_current = 0;

	CStringA m_message;

public:
	Profiler();

	void frameStart();
	void mark( EffectProfiler* fx );
	void frameEnd();
};

Profiler& gpuProfiler();