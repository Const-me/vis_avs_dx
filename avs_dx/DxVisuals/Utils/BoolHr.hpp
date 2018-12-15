#pragma once

// Utility class that can have 3 states: failed, true or false. True is stored as S_OK, false as S_FALSE.
class BoolHr
{
	HRESULT m_hr;

public:
	// Construct with bool, the default is false.
	BoolHr( bool b = false )
	{
		m_hr = b ? S_OK : S_FALSE;
	}
	// Construct with HRESULT
	BoolHr( HRESULT hr )
	{
		m_hr = hr;
		if( SUCCEEDED( hr ) && hr != S_FALSE )
			m_hr = S_OK;
	}

	// Combine with HRESULT code, returns true if the result is failed
	bool combine( HRESULT hr )
	{
		if( FAILED( m_hr ) )
			return true;
		if( FAILED( hr ) )
		{
			m_hr = hr;
			return true;
		}
		if( S_FALSE != hr )
			m_hr = S_OK;
		return false;
	}

	// Combine with bool value, returns true if the result is failed
	bool combine( bool b )
	{
		if( FAILED( m_hr ) )
			return true;
		if( b )
			m_hr = S_OK;
		return false;
	}

	bool failed() const
	{
		return FAILED( m_hr );
	}
	bool succeeded() const
	{
		return SUCCEEDED( m_hr );
	}
	operator HRESULT() const
	{
		return m_hr;
	}
	bool value() const
	{
		assert( succeeded() );
		return m_hr == S_OK;
	}
};