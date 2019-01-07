#include "stdafx.h"
#include <compressapi.h>
#include "decompressor.h"
#pragma comment( lib, "Cabinet.lib" )

namespace
{
	constexpr DWORD compressionAlgorithm = COMPRESS_ALGORITHM_MSZIP;

	class Decompressor
	{
		DECOMPRESSOR_HANDLE handle = nullptr;

		Decompressor()
		{
			if( CreateDecompressor( compressionAlgorithm, nullptr, &handle ) )
				return;
			logError( getLastHr(), "CreateDecompressor failed" );
			__debugbreak();
		}

	public:

		static const Decompressor& instance()
		{
			// It's used in static initializers, the order is undefined. Fortunately, modern C++ guarantees thread safety for such code.
			static const Decompressor s_decompressor;
			return s_decompressor;
		}

		void decompress( const uint8_t* src, int compressedLength, void* dest, int origLength ) const
		{
			if( Decompress( handle, src, (size_t)compressedLength, dest, (size_t)origLength, nullptr ) )
				return;
			logError( getLastHr(), "Decompress failed" );
			__debugbreak();
		}
	};
}

vector<uint8_t> decompressBytes( const uint8_t* src, int compressedLength, int origLength )
{
	vector<uint8_t> result;
	result.resize( (size_t)origLength );
	Decompressor::instance().decompress( src, compressedLength, result.data(), origLength );
	return eastl::move( result );
}

CStringA decompressString( const uint8_t* src, int compressedLength, int origLength )
{
	CStringA result;
	Decompressor::instance().decompress( src, compressedLength, result.GetBufferSetLength( origLength + 1 ), origLength );
	result.ReleaseBuffer( origLength );
	return eastl::move( result );
}