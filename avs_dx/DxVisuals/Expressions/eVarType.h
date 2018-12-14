#pragma once

namespace Expressions
{
	enum struct eVarType : uint8_t
	{
		unknown = 0,
		// 32-bit unsigned integer
		u32,
		// 32-bit floating point value
		f32,
		// 32-bit signed integer
		i32,
	};

	// HLSL name of the type
	const char* hlslName( eVarType vt );

	// Load HLSL expression, e.g. "asfloat( effectStates.Load( STATE_OFFSET + 8 ) )"
	CStringA stateLoad( eVarType vt, int offset );

	// Store HLSL expression, e.g. "effectStates.Store( STATE_OFFSET + 12, asuint( varName ) )"
	CStringA stateStore( eVarType vt, int offset, const char* varName );
}