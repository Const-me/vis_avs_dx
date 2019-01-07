#include "stdafx.h"
#include "Profiler.h"
#include <Effects/Common/EffectBase.h>
#include <Utils/PerfMeasure.h>

HRESULT EffectProfiler::create()
{
	CD3D11_QUERY_DESC desc{ D3D11_QUERY_TIMESTAMP };
	for( uint8_t i = 0; i < profilerBuffersCount; i++ )
		CHECK( device->CreateQuery( &desc, &m_queries[ i ] ) );
	return S_OK;
}

EffectProfiler::EffectProfiler( const char* name ) :
	m_name( name )
{
	create();
}

EffectProfiler::EffectProfiler( EffectBase* fx ) :
	m_name( fx->metadata().name )
{
	create();
}

extern CComAutoCriticalSection renderLock;

EffectProfiler::~EffectProfiler()
{
	CSLock __lock( renderLock );
	gpuProfiler().removeEffect( this );
}

void EffectProfiler::mark()
{
	gpuProfiler().mark( this );
}

Profiler::Profiler()
{
	for( auto& q : m_frames )
		q.create();
}

void Profiler::shutdown()
{
	for( auto& q : m_frames )
		q.destroy();
}

void Profiler::frameStart()
{
	m_running = isProfilerOpen();
	m_frame++;
	if( !m_running )
		return;
	m_level = 0;
	m_frames[ m_buffer ].frameStart();
}

void Profiler::mark( EffectProfiler* fx )
{
	if( !m_running )
		return;
	m_frames[ m_buffer ].mark( m_buffer, m_level, fx );
}

void Profiler::frameEnd()
{
	if( !m_running )
		return;

	m_frames[ m_buffer ].frameEnd();

	m_buffer = ( m_buffer + 1 ) % profilerBuffersCount;
	m_frames[ m_buffer ].report( m_frame, m_result, m_buffer );
}



void Profiler::removeEffect( EffectProfiler* pfx )
{
	for( auto& f : m_frames )
		f.removeEffect( pfx );
}

Profiler& gpuProfiler()
{
	static Profiler s_profiler;
	return s_profiler;
}