#include "stdafx.h"
#include "mfStatic.h"
#include <mfapi.h>
#pragma comment( lib, "Mfplat.lib" )
#pragma comment( lib, "Mf.lib" )

namespace
{
	class Startup
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

		~Startup()
		{
			if( started )
			{
				MFShutdown();
				started = false;
			}
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

		~CoInit()
		{
			if( started )
			{
				CoUninitialize();
				started = false;
			}
		}
	};
}

HRESULT mfStartup()
{
	// The documentation doesn't say we must MFSHutdown when a thread quits. However, it's logical thing to do, otherwise the order CoUninitialize -> MFShutdown is probably wrong.
	// BTW, C++ runtime guarantees correct destruction order for thread_local variables.
	static thread_local Startup s_startup;
	return s_startup.startup();
}

HRESULT coInit()
{
	static thread_local CoInit s_coInit;
	return s_coInit.startup();
}

/* HRESULT mfSourceResolver( CComPtr<IMFSourceResolver>& resolver )
{
	static CComPtr<IMFSourceResolver> res;
	if( res )
	{
		resolver = res;
		return S_FALSE;
	}

	CHECK( MFCreateSourceResolver( &res ) );
	resolver = res;
	return S_OK;
} */

HRESULT mfEngineFactory( CComPtr<IMFMediaEngineClassFactory>& factory )
{
	static thread_local CComPtr<IMFMediaEngineClassFactory> res;
	if( res )
	{
		factory = res;
		return S_FALSE;
	}

	CHECK( res.CoCreateInstance( CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER ) );
	factory = res;
	return S_OK;
}