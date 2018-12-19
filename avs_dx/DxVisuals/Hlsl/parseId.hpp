#pragma once

namespace Hlsl
{
	// true if the character is a letter A to Z or a to z or an underscore
	inline bool isAlpha( char c )
	{
		return isalpha( c ) || c == '_';
	}

	// true if the character is a letter A to Z or a to z or an underscore or a digit
	inline bool isAlphaNumeric( char c )
	{
		return isalnum( c ) || c == '_';
	}
}