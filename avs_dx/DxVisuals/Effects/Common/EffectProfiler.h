#pragma once
#if GPU_PROFILE
#include <Render/Profiler.h>
#else
struct DisabledEffectProfiler
{
	EffectProfiler( const char* name ) { }
	DisabledEffectProfiler( EffectBase* fx ) { }

	void mark() { }
};

using EffectProfiler = DisabledEffectProfiler;

struct ProfilerLevel { };

#endif