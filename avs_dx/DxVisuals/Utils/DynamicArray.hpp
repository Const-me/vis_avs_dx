#pragma once

// Very similar to std::array but can contain less elements.
template<class T, uint8_t capacity>
class DynamicArray
{
	std::array<T, capacity> m_data;
	uint8_t m_count = 0;

public:
	DynamicArray() = default;
	DynamicArray( const DynamicArray &that ) = default;
	DynamicArray( T t )
	{
		m_count = 1;
		m_data[ 0 ] = t;
	}

	void add( T v )
	{
		assert( m_count < capacity );
		m_data[ m_count ] = v;
		m_count++;
	}

	bool addIfNotThere( T newVal )
	{
		for( T v : *this )
			if( v == newVal )
				return false;
		add( newVal );
		return true;
	}

	decltype( auto ) begin() const
	{
		return m_data.begin();
	};

	decltype( auto ) end() const
	{
		return m_data.begin() + m_count;
	};

	uint8_t size() const
	{
		return m_count;
	}

	T& operator[]( uint8_t i )
	{
		assert( i < m_count );
		return m_data[ i ];
	}

	T operator[]( uint8_t i ) const
	{
		assert( i < m_count );
		return m_data[ i ];
	}
};