#pragma once

// For this application, we don't actually need a cryptography-grade hash algorithm. It's used to detect changes in shader codes, or in color map's data.
// However, MD4 is quite fast, it takes about 5-6us (microseconds) to hash a string or a small buffer.
// It's included in Windows so we don't have to develop nor support a custom implementation.

// Calculate MD4 hash of a string. This function is not thread safe.
__m128i hashString( const CStringA& str );

__m128i hashBuffer( const void* pv, size_t cb );

// Utility function to compare two 16-byte registers.
inline bool operator==( __m128i a, __m128i b )
{
	return 0xFFFF == _mm_movemask_epi8( _mm_cmpeq_epi32( a, b ) );
}

inline bool operator!=( __m128i a, __m128i b )
{
	return !( a == b );
}