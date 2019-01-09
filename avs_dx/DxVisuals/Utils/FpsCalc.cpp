#include "stdafx.h"
#include "FpsCalc.h"
#include "preciseTickCount.h"

constexpr float ticksPerSecond = 1E+6;
static const float s_mulToTicks = ticksPerSecond / (float)queryPerformanceFrequency();

float FpsCalc::update()
{
	const uint64_t qpcNow = queryPerformanceCounter();

	if( qpcPrev > 0 )
	{
		const uint64_t thisFrame = qpcNow - qpcPrev;
		qpcPrev = qpcNow;

		const uint32_t ticks = (uint32_t)lround( (float)thisFrame * s_mulToTicks );
		if( m_buffer.size() >= measures )
		{
			totalSum -= m_buffer[ nextIndex ];
			totalSum += ticks;
			m_buffer[ nextIndex ] = ticks;
			nextIndex = ( nextIndex + 1 ) % measures;

			constexpr float resMul = ticksPerSecond * measures;
			return resMul / totalSum;
		}

		m_buffer.add( ticks );
		totalSum += ticks;
		return ticksPerSecond * m_buffer.size() / totalSum;
	}

	qpcPrev = qpcNow;
	return 0;
}