#include "stdafx.h"
#include "ProfilerAverages.h"

ProfilerAverages::ProfilerAverages() :
	m_map( 17, 0.75f, 0.25f, 1.5f, 64 )
{ }

bool ProfilerAverages::update( const std::vector<sProfilerEntry>& entries )
{
	for( POSITION pos = m_map.GetStartPosition(); pos;)
	{
		sEntry& e = m_map.GetNextValue( pos );
		e.present = false;
	}

	bool updated = false;
	m_result.resize( entries.size() );
	for( size_t i = 0; i < entries.size(); i++ )
	{
		const sProfilerEntry& e = entries[ i ];
		auto p = m_map.Lookup( e.measureInstance );
		if( nullptr != p )
			m_result[ i ] = updateEntry( p->m_value, e.milliseconds );
		else
		{
			m_result[ i ] = createEntry( e.measureInstance, e.milliseconds );
			updated = true;
		}
	}

	m_drop.clear();
	for( POSITION pos = m_map.GetStartPosition(); pos;)
	{
		auto p = m_map.GetNext( pos );
		if( !p->m_value.present )
		{
			m_drop.push_back( p );
			updated = true;
		}
	}
	for( auto p : m_drop )
		m_map.RemoveAtPos( p );
	return updated;
}

inline uint32_t intTime( float ms )
{
	assert( ms >= 0 );
	return (uint32_t)lroundf( ms * 1000.0f );
}

float ProfilerAverages::createEntry( const void* key, float current )
{
	assert( nullptr == m_map.Lookup( key ) );
	sEntry& e = m_map[ key ];

	assert( e.data.size() == 0 );
	const uint32_t us = intTime( current );
	e.data.add( us );
	e.total = us;
	e.next = 1;
	e.present = true;
	return current;
}

float ProfilerAverages::updateEntry( sEntry& e, float current )
{
	e.present = true;
	const uint32_t us = intTime( current );

	if( e.data.size() >= framesCount )
	{
		e.total = e.total - e.data[ e.next ] + us;
		e.data[ e.next ] = us;
		e.next = ( e.next + 1 ) % framesCount;
		constexpr float mul = (float)( 1.0 / ( framesCount * 1000.0 ) );
		return mul * (float)e.total;
	}
	else
	{
		e.total += us;
		e.data.add( us );
		e.next = ( e.next + 1 ) % framesCount;
		const float mul = 1.0f / (float)( (uint32_t)e.data.size() * 1000 );
		return mul * (float)e.total;
	}
}