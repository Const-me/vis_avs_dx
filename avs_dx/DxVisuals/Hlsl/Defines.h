#pragma once

namespace Hlsl
{
	class Defines
	{
		std::vector<std::pair<CStringA, CStringA>> m_map;

	public:
		Defines() = default;
		Defines( Defines* parentScope ) : m_map( parentScope->m_map ) { }

		void set( const CStringA& name, const CStringA& value );

		static void getData( const Defines* pDefines, std::vector<D3D_SHADER_MACRO>& macros );
	};
}