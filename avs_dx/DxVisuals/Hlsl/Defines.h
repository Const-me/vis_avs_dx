#pragma once

namespace Hlsl
{
	// A set of HLSL macros with their values.
	class Defines
	{
		std::vector<std::pair<CStringA, CStringA>> m_map;

	public:
		Defines() = default;
		Defines( Defines* parentScope ) : m_map( parentScope->m_map ) { }

		void set( const CStringA& name, const CStringA& value );

		void clear() { m_map.clear(); }

		void set( const CStringA &key, uint32_t value );
		void set( const CStringA &key, int value );
		void set( const CStringA &key, float value );

		// Add macro with values like "0x11, 0x22, 0x33"
		void set( const CStringA &key, const std::vector<int>& vec );

		// For HLSL-running macros
		std::vector<D3D_SHADER_MACRO> data() const;

		// For manually running macros
		CStringA expand( CStringA hlsl ) const;
	};
}