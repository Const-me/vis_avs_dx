#pragma once

namespace Hlsl
{
	// When assembling state shaders, we can't use HLSL's macro system: different instances of the same effect would cause name conflicts.
	// Replacing strings in HLSL source code instead.
	// This class contains macros and their values for that.
	class StateMacroValues
	{
		std::vector<std::pair<CStringA, CStringA>> m_map;

	public:
		StateMacroValues( int stateBufferOffset );

		void add( const CStringA &key, const CStringA &value );

		void add( const CStringA &key, int value );

		CStringA expand( CStringA hlsl ) const;
	};
}