#include "stdafx.h"
#include "FadeOut.h"

HRESULT FadeOut::update( int lengthInFrames )
{
	if( m_frames == lengthInFrames )
		return S_FALSE;
	if( lengthInFrames <= 0 )
		lengthInFrames = 1;

	m_frames = lengthInFrames;
	m_mul = 1.0f / lengthInFrames;
	m_counter = 0;
	return S_OK;
}

float FadeOut::value( bool isBeat ) const
{
	if( isBeat )
		m_counter = m_frames;
	else if( m_counter > 0 )
		m_counter--;
	else
		return 0;

	const float y = m_mul * ( m_frames - m_counter );
	// 6-degree minimax approximation, copy-pasted from XMScalarCosEst in DirectXMathMisc.inl (MIT license, which is compatible with copy & paste)
	const float y2 = y * y;
	return ( ( -0.0012712436f * y2 + 0.041493919f ) * y2 - 0.49992746f ) * y2 + 1.0f;
}