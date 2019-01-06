#pragma once

namespace Expressions
{
	// Type of a variable
	enum struct eVarType : uint8_t
	{
		unknown = 0,
		// 32-bit unsigned integer
		u32,
		// 32-bit floating point value
		f32,
		// 32-bit signed integer
		i32,
		// double-precision float
		f64,

		count
	};

	// HLSL name of the type
	const char* hlslName( eVarType vt );

	// Count of 4-byte items used by this type
	int variableSize( eVarType vt );

	// Load HLSL expression, e.g. "asfloat( effectStates.Load( STATE_OFFSET + 8 ) )". The offset is in 4-byte items.
	CStringA stateLoad( eVarType vt, int offset );

	// Store HLSL expression, e.g. "effectStates.Store( STATE_OFFSET + 12, asuint( varName ) )". The offset is in 4-byte items.
	CStringA stateStore( eVarType vt, int offset, const char* varName );

	// How the variable is used by the code, a 2 bits mask
	enum struct eVarAccess : uint8_t
	{
		None = 0,
		Read = 1,
		Write = 2,
		ReadWrite = 3,
	};

	// Where the variable is located
	enum struct eVarLocation : uint8_t
	{
		unknown,

		// It's not a variable at all but macro, will be compiled into immediate value in DXBC
		macro,
		// Statically allocated in the state by effect prototype
		stateStatic,
		// Dynamically allocated in the state by NSEEL code
		stateDynamic,

		// Set by hardcoded part of the fragment code
		fragmentInput,

		// Set by hardcoded part of the fragment code, and used by another hardcoded part to produce the output vertex/pixel.
		fragmentOutput,

		// Local variables that doesn't go anywhere
		stateLocal,
		fragmentLocal,

		valuesCount
	};
}