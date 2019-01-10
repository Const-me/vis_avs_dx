#include "stdafx.h"
#include "convolutionHacks.h"

KernelData getKernelData( const C_RBASE* pConvFx )
{
	KernelData res;
	const uint8_t* const ptr = (const uint8_t*)pConvFx;

	res.wrap = !!ptr[ 0x459 ];

	const int scale = *(const int*)( ptr + 0x524 );
	const int * const firstValue = (const int*)( ptr + 0x45C );

	const float mul = 1.0f / (float)scale;
	for( int i = 0; i < 49; i++ )
		res.values[ i ] = (float)firstValue[ i ] * mul;

	return res;
}

MiscData getMiscData( const C_RBASE* pConvFx )
{
	MiscData res;
	const uint8_t* const ptr = (const uint8_t*)pConvFx;

	res.enabled = !!ptr[ 0x458 ];
	res.twoPasses = !!ptr[ 0x45B ];

	return res;
}