#pragma once

namespace BorderHacks
{
	inline bool isEnabled( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return !!ptr[ 4 ];
	}
	inline int getColor( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return *(int*)( ptr + 8 );
	}
	inline int getWidth( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return *(int*)( ptr + 12 );
	}
}