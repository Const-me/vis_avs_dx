#include "stdafx.h"
#include "ApeEffectBase.h"
#include <Interop/effectsFactory.h>
#include <Utils/DynamicArray.hpp>
#include <atlpath.h>

constexpr uint8_t apeEffectsCount = 4;

namespace
{
	struct sApeEffect
	{
		const char* ape;
		const char* name;
		ApeEffectBase::pfnCreateEffect pfnCreate;
	};

	DynamicArray<sApeEffect, apeEffectsCount> &factories()
	{
		static DynamicArray<sApeEffect, apeEffectsCount> s_factories;
		return s_factories;
	}

	HRESULT getModuleFileName( HINSTANCE hDllInstance, CStringA& result )
	{
		CPathA dllPath;
		const DWORD ret = GetModuleFileNameA( hDllInstance, dllPath.m_strPath.GetBufferSetLength( MAX_PATH ), MAX_PATH );
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

ApeEffectBase::Metadata::Metadata( const char* name, const char* apeName, pfnCreateEffect classFactory ) :
	EffectBase::Metadata( apeName, false )
{
	// __debugbreak();
	sApeEffect fx{ apeName, name, classFactory };
	factories().add( fx );
}

HRESULT ApeEffectBase::create( HINSTANCE hDllInstance, const char* nameEffect, C_RBASE* pThis )
{
	CStringA nameDll;
	CHECK( getModuleFileName( hDllInstance, nameDll ) );

	for( const sApeEffect &f : factories() )
	{
		if( 0 != nameDll.CompareNoCase( f.ape ) )
			continue;
		if( nullptr != f.name && 0 != lstrcmpiA( nameEffect, f.name ) )
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