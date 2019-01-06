#pragma once

namespace MapHacks
{
	// byte [8] = int[2]
	enum struct eMapSourceChannel : uint8_t
	{
		Red = 0,
		Green = 1,
		Blue = 2,
		HalfSum = 3,
		Maximum = 4,
		ThirdSum = 5,
	};

	// byte [12] = int[3]
	enum struct eMapBlendMode : uint8_t
	{
		Replace = 0,
		Additive = 1,
		Maximum = 2,
		Minimum = 3,
		Fifty = 4,
		Subtractive1 = 5,
		Subtractive2 = 6,
		Multiply = 7,
		Xor = 8,
		Adjustable = 9,
	};

	inline eMapSourceChannel getSourceChannel( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return (eMapSourceChannel)ptr[ 8 ];
	}

	inline eMapBlendMode getDestBlendMode( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return (eMapBlendMode)ptr[ 12 ];
	}

	inline uint8_t getAdjustableValue( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return ptr[ 20 ];
	}

	inline uint8_t getCurrentMapId( const C_RBASE* pEffect )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return ptr[ 0x1F8 ];	// Same value is at 0x1FC
	}

	inline bool isEnabled( const C_RBASE* pEffect, uint8_t id )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return !!ptr[ 0x18 + id * 0x3C ];
	}

	// Pointer to the color table, BGRA format, 256 elements size. Alpha is zero.
	inline const uint32_t* getTablePointer( const C_RBASE* pEffect, uint8_t id )
	{
		const uint8_t* const ptr = (const uint8_t*)pEffect;
		return (const uint32_t*)( ptr + 0x208 + (size_t)0x400 * (size_t)id );
	}
}