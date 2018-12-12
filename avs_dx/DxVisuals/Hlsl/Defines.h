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

		std::vector<D3D_SHADER_MACRO> data() const;
	};
}