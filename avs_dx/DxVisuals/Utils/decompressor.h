#pragma once

std::vector<uint8_t> decompressBytes( const uint8_t* src, int compressedLength, int origLength );

CStringA decompressString( const uint8_t* src, int compressedLength, int origLength );