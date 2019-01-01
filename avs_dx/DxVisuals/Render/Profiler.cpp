#include "stdafx.h"
#include "Profiler.h"
#include "../EffectBase/EffectBase.h"
#include <Utils/PerfMeasure.h>

constexpr bool busyWaiting = false;

inline void waitForData()
{
	if( busyWaiting )
	{
		for( int i = 0; i < 1024; i++ )
			_mm_pause();
	}
	else
		Sleep( 1 );
}

HRESULT EffectProfiler::create()
{
	CD3D11_QUERY_DESC desc{ D3D11_QUERY_TIMESTAMP };
	for( auto& q : m_queries )
		CHECK( device->CreateQuery( &desc, &q ) );
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

HRESULT Profiler::FrameData::create()
{
	disjoint = begin = end = nullptr;

	CD3D11_QUERY_DESC desc{ D3D11_QUERY_TIMESTAMP };
	CHECK( device->CreateQuery( &desc, &begin ) );
	CHECK( device->CreateQuery( &desc, &end ) );
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	CHECK( device->CreateQuery( &desc, &disjoint ) );

	return S_OK;
}

Profiler::Profiler()
{
	for( auto& q : m_frames )
		q.create();
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

void Profiler::FrameData::frameStart()
{
	context->Begin( disjoint );
	context->End( begin );
	haveMeasures = true;
	effects.clear();
}

void Profiler::FrameData::mark( uint8_t current, uint32_t level, EffectProfiler* fx )
{
	ID3D11Query* q = fx->m_queries[ current ];
	context->End( q );
	effects.emplace_back( sEntry{ fx, level } );
}

void Profiler::FrameData::frameEnd()
{
	context->End( end );
	context->End( disjoint );
}

namespace
{
	inline HRESULT getTimestamp( ID3D11Query* q, uint64_t& res )
	{
		while( true )
		{
			const HRESULT hr = context->GetData( q, &res, sizeof( uint64_t ), 0 );
			CHECK( hr );
			if( S_OK == hr )
				return S_OK;
			waitForData();
		}
	}
}

HRESULT Profiler::FrameData::report( uint32_t frame, std::vector<sProfilerEntry> &result, uint8_t buffer )
{
	if( !haveMeasures )
		return S_FALSE;
	haveMeasures = false;
	if( !isProfilerOpen() )
		return S_FALSE;

	// PerfMeasure cpuPerf( "Profiler::FrameData::report" );
	while( true )
	{
		const HRESULT hr = context->GetData( disjoint, nullptr, 0, 0 );
		CHECK( hr );
		if( hr == S_OK )
			break;
		waitForData();
	}

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	CHECK( context->GetData( disjoint, &tsDisjoint, sizeof( tsDisjoint ), 0 ) );
	if( tsDisjoint.Disjoint )
		return S_FALSE;

	const float msMul = (float)( 1000.0 / tsDisjoint.Frequency );
	uint64_t tsPrev;
	CHECK( getTimestamp( begin, tsPrev ) );

	auto produceEntry = [ &result, &tsPrev, msMul ]( uint64_t ts, const void* pEffect, uint32_t level, const char* name )
	{
		const float ms = (float)( ts - tsPrev ) * msMul;
		result.emplace_back( sProfilerEntry{ pEffect, level, name, ms } );
		tsPrev = ts;
	};

	result.clear();
	uint64_t tsCurr;
	for( auto e : effects )
	{
		CHECK( getTimestamp( e.pfx->m_queries[ buffer ], tsCurr ) );
		produceEntry( tsCurr, e.pfx, e.level, e.pfx->m_name );
	}

	CHECK( getTimestamp( end, tsCurr ) );
	produceEntry( tsCurr, &gpuProfiler(), 0, "present" );

	return updateProfilerGui( frame, result ) ? S_OK : S_FALSE;
}

void Profiler::removeEffect( EffectProfiler* pfx )
{
	for( auto& f : m_frames )
		f.removeEffect( pfx );
}

void Profiler::FrameData::removeEffect( EffectProfiler* pfx )
{
	// This happens very rarely, i.e. dynamic memory is fine.
	std::vector<sEntry> result;
	result.reserve( effects.size() );
	bool found = false;
	for( auto e : effects )
	{
		if( e.pfx == pfx )
			found = true;
		else
			result.push_back( e );
	}
	if( found )
		effects.swap( result );
}

Profiler& gpuProfiler()
{
	static Profiler s_profiler;
	return s_profiler;
}