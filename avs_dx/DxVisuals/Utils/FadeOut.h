#pragma once

// Implements y=cos(x) table, only without a table i.e. much faster.
class FadeOut
{
	int m_frames;
	float m_mul;
	mutable int m_counter;

public:

	FadeOut()
	{
		update( 1 );
	}

	HRESULT update( int lengthInFrames );

	float value( bool isBeat ) const;
};