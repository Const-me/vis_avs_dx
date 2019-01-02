#pragma once

template<class T>
class LinearTransition
{
	T m_target, m_initial;
	float m_mul = 1.0f;
	mutable uint32_t m_framesLeft = 0;

	T getCurrent() const
	{
		const float v = m_mul * (float)m_framesLeft;
		return m_initial * v + m_target * ( 1.0f - v );
	}

public:

	LinearTransition() = default;
	LinearTransition( T init ) : m_initial( init ), m_target( init ) { }

	T target() const
	{
		return m_target;
	}

	void setTarget( uint32_t frames, T target )
	{
		if( 0 == frames )
		{
			m_target = m_initial = target;
			m_mul = 1.0f;
			m_framesLeft = 0;
			return;
		}

		m_initial = getCurrent();
		m_framesLeft = frames;
		m_mul = 1.0f / (float)frames;
		m_target = target;
	}

	bool finished() const
	{
		return m_framesLeft == 0;
	}

	T update() const
	{
		if( 0 == m_framesLeft )
			return m_target;
		m_framesLeft--;
		return getCurrent();
	}
};