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

	public:
		Decompressor()
		{
			if( CreateDecompressor( compressionAlgorithm, nullptr, &handle ) )
				return;
			logError( getLastHr(), "CreateDecompressor failed" );
			__debugbreak();
		}

		void decompress( const uint8_t* src, int compressedLength, void* dest, int origLength ) const
		{
			if( Decompress( handle, src, (size_t)compressedLength, dest, (size_t)origLength, nullptr ) )
				return;
			logError( getLastHr(), "Decompress failed" );
			__debugbreak();
		}
	};

	static const Decompressor s_decompressor;
}

std::vector<uint8_t> decompressBytes( const uint8_t* src, int compressedLength, int origLength )
{
	std::vector<uint8_t> result;
	result.resize( (size_t)origLength );
	s_decompressor.decompress( src, compressedLength, result.data(), origLength );
	return std::move( result );
}

CStringA decompressString( const uint8_t* src, int compressedLength, int origLength )
{
	CStringA result;
	s_decompressor.decompress( src, compressedLength, result.GetBufferSetLength( origLength + 1 ), origLength );
	result.ReleaseBuffer( origLength );
	return std::move( result );
}