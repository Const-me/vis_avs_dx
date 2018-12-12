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

		void add( const CStringA &key, const CStringA &value );

		void add( const CStringA &key, int value );

		// Add macro with values like "0x11, 0x22, 0x33"
		void uintConstants( const CStringA &key, size_t count, const int* arr );

		void uintConstants( const CStringA &key, const std::vector<int>& vec )
		{
			if( !vec.empty() )
				uintConstants( key, vec.size(), vec.data() );
			else
				add( key, "" );
		}

		CStringA expand( CStringA hlsl ) const;
	};
}