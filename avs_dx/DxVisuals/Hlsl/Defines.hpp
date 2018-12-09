#pragma once

namespace Hlsl
{
	class Defines
	{
		std::vector<std::pair<CStringA, CStringA>> m_map;

	public:
		Defines() = default;
		Defines( Defines* parentScope ) : m_map( parentScope->m_map ) { }

		void set( const char* name, const char* value )
		{
			const CStringA key = name;
			for( auto& p : m_map )
			{
				if( p.first == key )
				{
					// TODO [low]: maybe a warning about redefining the macro?
					p.second = value;
					return;
				}
			}
			m_map.emplace_back( std::make_pair( key, CStringA{ value } ) );
		}

		static void getData( Defines* pDefines, std::vector<D3D_SHADER_MACRO>& macros )
		{
			macros.clear();
			if( nullptr != pDefines )
			{
				macros.reserve( pDefines->m_map.size() + 1 );
				for( auto& p : pDefines->m_map )
					macros.emplace_back( D3D_SHADER_MACRO{ p.first, p.second } );
			}
			macros.emplace_back( D3D_SHADER_MACRO{ nullptr, nullptr } );
		}
	};
}