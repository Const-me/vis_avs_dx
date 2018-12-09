#pragma once

namespace Hlsl
{
	enum struct eVarType : uint8_t
	{
		// 32-bit floating point value. Vectors are supported, i.e. it may be from 1 to 4 lanes wide.
		f32,
		// 32-bit signed integer. Vectors are supported, i.e. it may be from 1 to 4 lanes wide.
		i32,
		// 32-bit unsigned integer. Vectors are supported, i.e. it may be from 1 to 4 lanes wide.
		u32,
		// true or false. Vectors are not supported, must be a scalar.
		boolean,
		// 64-bit floating point value. Vectors are not supported, must be a scalar.
		f64,

		CountTypes,
	};

	struct Variable
	{
		// Name of the variable. Size is limited to 8 because that's the limit in the original code.
		std::array<char, 8> name;

		// Type of the variable
		eVarType vt;

		// Width of the variable, int and floats support 1-4, bool and double must be 1.
		uint8_t width;

		// Validate name + width
		bool validate() const;

		uint8_t bytesCount() const;

		static Variable floatScalar( const char* name );
		static Variable float2( const char* name );
	};

	struct PackedVariable : public Variable
	{
		// From 0 to 3, 0 means ".x", 3 means ".w"
		uint8_t component;
		// packoffset, starting from the begin of the buffer.
		uint16_t offset;
		// 0-based index of the effect that uses this variable
		uint16_t effect;
	};

	// Pack variables i.e. calculate offsets
	HRESULT appendPacked( const std::vector<Variable> &src, uint16_t effect, uint16_t &startOffset, std::vector<PackedVariable> &dest );

	// Append packed variables to HLSL structure definition
	// void hlslStructure( const std::vector<PackedVariable>& vars, int effect, uint16_t startOffset, CStringA& hlsl );
}