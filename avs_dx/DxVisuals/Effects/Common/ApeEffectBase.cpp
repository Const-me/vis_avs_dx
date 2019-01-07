#include "stdafx.h"
#include "ApeEffectBase.h"
#include <Interop/effectsFactory.h>
#include <Utils/DynamicArray.hpp>
#include <atlpath.h>

namespace
{
	struct sApeEffect
	{
		const wchar_t* ape;
		const char* name;
		ApeEffectBase::pfnCreateEffect pfnCreate;
	};

	static DynamicArray<sApeEffect, 2> s_factories;

	HRESULT getModuleFileName( HINSTANCE hDllInstance, CString& result )
	{
		CPath dllPath;
		const DWORD ret = GetModuleFileNameW( hDllInstance, dllPath.m_strPath.GetBufferSetLength( MAX_PATH ), MAX_PATH );
		if( 0 == ret )
		{
			dllPath.m_strPath.ReleaseBuffer();
			return getLastHr();
		}
		dllPath.m_strPath.ReleaseBufferSetLength( (int)ret );
		const int fn = dllPath.FindFileName();
		if( fn < 0 )
			return E_FAIL;
		result = dllPath.m_strPath.Mid( fn );
		return S_OK;
	}
}

ApeEffectBase::Metadata::Metadata( const char* name, LPCTSTR apeName, pfnCreateEffect classFactory ) :
	EffectBase::Metadata( name, false )
{
	// __debugbreak();
	sApeEffect fx{ apeName, name, classFactory };
	s_factories.add( fx );
}

HRESULT ApeEffectBase::create( HINSTANCE hDllInstance, const char* nameEffect, C_RBASE* pThis )
{
	CString nameDll;
	CHECK( getModuleFileName( hDllInstance, nameDll ) );

	for( const sApeEffect &f : s_factories )
	{
		if( 0 != nameDll.CompareNoCase( f.ape ) )
			continue;
		if( 0 != lstrcmpiA( nameEffect, f.name ) )
			continue;
		return f.pfnCreate( pThis );
	}
	return E_INVALIDARG;
}

bool createApeEffect( HINSTANCE hDllInstance, const char* name, C_RBASE* pThis )
{
	const HRESULT hr = ApeEffectBase::create( hDllInstance, name, pThis );
	return SUCCEEDED( hr );
}