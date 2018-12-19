#pragma once
#include "../Hlsl/parseId.hpp"

namespace Expressions
{
	inline const char* cstr( const CStringA&s ) { return s; }

	template<class TContainer, class Pred>
	inline bool hasAny( const TContainer& c, Pred p )
	{
		return std::find_if( std::begin( c ), std::end( c ), p ) != std::end( c );
	}

	using Hlsl::isAlpha;
	using Hlsl::isAlphaNumeric;
}