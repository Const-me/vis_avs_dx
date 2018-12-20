#pragma once

struct ByteRange
{
	const uint8_t* data = nullptr;
	uint32_t size = 0;

	ByteRange() = default;
	constexpr ByteRange( const uint8_t* p, size_t s ) :
		data( p ), size( (uint32_t)s ) { }
};