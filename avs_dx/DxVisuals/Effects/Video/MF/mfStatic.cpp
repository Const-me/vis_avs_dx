#include "stdafx.h"
#include "mfStatic.h"
#include <mfapi.h>
#pragma comment( lib, "Mfplat.lib" )
#pragma comment( lib, "Mf.lib" )

namespace
{
	class MfStartup
	{
		bool started = false;

	public:
		HRESULT startup()
		{
			if( started )
				return S_FALSE;
			CHECK( MFStartup( MF_VERSION, MFSTARTUP_NOSOCKET ) );
			started = true;
			return S_OK;
		}

		~MfStartup()
		{
			shutdown();
		}

		HRESULT shutdown()
		{
			if( started )
			{
				logShutdown( "MFShutdown" );
				MFShutdown();
				started = false;
				return S_OK;
			}
			return S_FALSE;
		}
	};

	class CoInit
	{
		bool started = false;

	public:
		HRESULT startup()
		{
			if( started )
				return S_FALSE;
			CHECK( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) );
			started = true;
			return S_OK;
		}

		HRESULT shutdown()
		{
			if( started )
			{
				logShutdown( "CoUninitialize" );
				CoUninitialize();
				started = false;
				return S_OK;
			}
			return S_FALSE;
		}
	};

	CoInit g_com;
	MfStartup g_mf;
	CComPtr<IMFMediaEngineClassFactory> g_mecf;
}

HRESULT mfStartup()
{
	return g_mf.startup();
}

HRESULT coInit()
{
	return g_com.startup();
}

HRESULT mfEngineFactory( CComPtr<IMFMediaEngineClassFactory>& factory )
{
	if( g_mecf )
	{
		factory = g_mecf;
		return S_FALSE;
	}

	CHECK( g_mecf.CoCreateInstance( CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER ) );
	factory = g_mecf;
	return S_OK;
}

HRESULT mfShutdown()
{
	return g_mf.shutdown();
}

HRESULT comUninitialize()
{
	return g_com.shutdown();
}