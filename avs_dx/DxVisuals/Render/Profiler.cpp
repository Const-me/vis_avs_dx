#include "stdafx.h"
#include "Profiler.h"
#include "../EffectBase/EffectBase.h"

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
	m_frame++;
	m_level = 0;
	m_frames[ m_buffer ].frameStart();
}

void Profiler::mark( EffectProfiler* fx )
{
	m_frames[ m_buffer ].mark( m_buffer, m_level, fx );
}

void Profiler::frameEnd()
{
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
		return context->GetData( q, &res, sizeof( uint64_t ), 0 );
	}
}

HRESULT Profiler::FrameData::report( uint32_t frame, std::vector<sProfilerEntry> &result, uint8_t buffer )
{
	if( !haveMeasures )
		return S_FALSE;
	haveMeasures = false;
	if( !isProfilerOpen() )
		return S_FALSE;

	while( true )
	{
		const HRESULT hr = context->GetData( disjoint, nullptr, 0, 0 );
		CHECK( hr );
		if( hr == S_FALSE )
		{
			Sleep( 1 );
			continue;
		}
		break;
	}

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	context->GetData( disjoint, &tsDisjoint, sizeof( tsDisjoint ), 0 );
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

Profiler& gpuProfiler()
{
	static Profiler s_profiler;
	return s_profiler;
}