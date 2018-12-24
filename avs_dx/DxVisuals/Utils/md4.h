#pragma once

__m128i hashString( const CStringA& str );

inline bool operator==( __m128i a, __m128i b )
{
	return 0xFFFF == _mm_movemask_epi8( _mm_cmpeq_epi32( a, b ) );
}