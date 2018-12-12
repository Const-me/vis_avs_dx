#include "stdafx.h"
#include "FunctionBuilder.h"

using namespace Hlsl;

void FunctionBuilder::print( const char* pszFormat, ... )
{
	va_list args;
	va_start( args, pszFormat );
	hlsl.AppendFormatV( pszFormat, args );
	va_end( args );
}

HRESULT FunctionBuilder::uintConstants( const char* constantName, const int* arr, int count )
{
	if( count <= 0 || count > 256 )
		return E_INVALIDARG;

	print( "	const uint %s[ %i ] = { ", constantName, count );
	for( int i = 0; i < count; i++ )
	{
		print( "0x%08X", arr[ i ] );
		if( i + 1 < count )
			hlsl += ", ";
		else
			hlsl += " };\r\n";
	}
	return S_OK;
}

void FunctionBuilder::print( const std::array<uint32_t, 4>& uint4 )
{
	print( "uint4( %08X, %08X, %08X, %08X )", uint4[ 0 ], uint4[ 1 ], uint4[ 2 ], uint4[ 3 ] );
}

HRESULT FunctionBuilder::uint4Constants( const char* constantName, const int* arr, int count )
{
	if( count <= 0 || count > 256 )
		return E_INVALIDARG;

	const int nVectors = count + 3 / 4;
	print( "	const uint4 %s[ %i ] = { ", constantName, nVectors );

	for( int i = 0; i < nVectors; i++ )
	{
		std::array<uint32_t, 4> uint4;
		for( int j = 0; j < 4; j++ )
		{
			const int ind = i * 4 + j;
			if( ind < count )
				uint4[ j ] = (uint32_t)arr[ ind ];
			else
				uint4[ j ] = (uint32_t)arr[ count - 1 ];
		}
		print( uint4 );
		if( i + 1 != nVectors )
			hlsl += ", ";
		else
			hlsl += " };\r\n";
	}
	return S_OK;
}