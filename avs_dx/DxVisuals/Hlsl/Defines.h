#pragma once
#include "../Expressions/variableTypes.h"

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
		void set( const CStringA &key, bool value );

		// Add macro with values like "0x11, 0x22, 0x33"
		void set( const CStringA &key, const std::vector<int>& vec );

		void set( const CStringA &key, const float2& value );
		void set( const CStringA &key, const float3& value );
		void set( const CStringA &key, const float4& value );

		// For HLSL-running macros, used by normal per-effect shaders.
		std::vector<D3D_SHADER_MACRO> data() const;

		// For manually expanded macros, used by state shaders.
		CStringA expand( CStringA hlsl ) const;
	};
}