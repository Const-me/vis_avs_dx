#pragma once
#include "../Hlsl/parseId.hpp"

namespace Expressions
{
	template<class TContainer, class Pred>
	inline bool hasAny( const TContainer& c, Pred p )
	{
		return std::find_if( std::begin( c ), std::end( c ), p ) != std::end( c );
	}

	using Hlsl::isAlpha;
	using Hlsl::isAlphaNumeric;
}