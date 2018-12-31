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
	m_message.Preallocate( 1024 );
}

void Profiler::frameStart()
{
	m_frames[ m_current ].frameStart();
}

void Profiler::mark( EffectProfiler* fx )
{
	m_frames[ m_current ].mark( m_current, fx );
}

void Profiler::frameEnd()
{
	m_frames[ m_current ].frameEnd();

	m_current = ( m_current + 1 ) % profilerBuffersCount;
	m_frames[ m_current ].report( m_message, m_current );
}

void Profiler::FrameData::frameStart()
{
	context->Begin( disjoint );
	context->End( begin );
	haveMeasures = true;
	effects.clear();
}

void Profiler::FrameData::mark( uint8_t current, EffectProfiler* fx )
{
	ID3D11Query* q = fx->m_queries[ current ];
	context->End( q );
	effects.push_back( fx );
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

HRESULT Profiler::FrameData::report( CStringA &message, uint8_t frame )
{
	if( !haveMeasures )
		return S_FALSE;
	haveMeasures = false;

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

	const double msMul = 1000.0 / tsDisjoint.Frequency;
	uint64_t tsPrev;
	CHECK( getTimestamp( begin, tsPrev ) );

	auto printTime = [ &, msMul ]( uint64_t ts, const char* name )
	{
		const double ms = ( ts - tsPrev )*msMul;
		message.AppendFormat( "%s %.4f; ", name, ms );
		tsPrev = ts;
	};

	message = "";
	uint64_t tsCurr;
	for( EffectProfiler* fx : effects )
	{
		CHECK( getTimestamp( fx->m_queries[ frame ], tsCurr ) );
		printTime( tsCurr, fx->m_name );
	}

	CHECK( getTimestamp( end, tsCurr ) );
	printTime( tsCurr, "final" );

	logInfo( "GPU perf: %s", cstr( message ) );
	return S_OK;
}

Profiler& gpuProfiler()
{
	static Profiler s_profiler;
	return s_profiler;
}