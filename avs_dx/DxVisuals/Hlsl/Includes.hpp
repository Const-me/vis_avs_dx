#pragma once

namespace Hlsl
{
	class Includes : public ID3DInclude
	{
		HRESULT __stdcall Open( D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes ) override
		{
			CStringA key = pFileName;
			key.MakeLower();
			const CAtlMap<CStringA, CStringA>& src = ( IncludeType == D3D_INCLUDE_LOCAL ) ? m_user : m_system;
			auto p = src.Lookup( key );
			if( nullptr != p )
			{
				*ppData = p->m_value.operator const char*( );
				*pBytes = p->m_value.GetLength();
				return S_OK;
			}

			if( nullptr != m_parentScope )
				return m_parentScope->Open( IncludeType, pFileName, pParentData, ppData, pBytes );

			return E_INVALIDARG;
		}

		HRESULT __stdcall Close( LPCVOID pData ) override { return S_OK; }

		CAtlMap<CStringA, CStringA> m_system, m_user;
		Includes* m_parentScope = nullptr;

		static void add( CAtlMap<CStringA, CStringA>& dest, const CStringA& name, const CStringA& hlsl )
		{
			CStringA key = name;
			key.MakeLower();
			dest[ key ] = hlsl;
		}

	public:

		Includes() = default;
		~Includes() = default;
		Includes( const Includes& that ) = delete;
		Includes( Includes* parentScope ) : m_parentScope( parentScope ) { }

		void addSystem( const CStringA& name, const CStringA& hlsl )
		{
			add( m_system, name, hlsl );
		}
		void addUser( const CStringA& name, const CStringA& hlsl )
		{
			add( m_user, name, hlsl );
		}
	};
}