#include "stdafx.h"
#include "mfStartup.h"
#include <mfapi.h>
#pragma comment( lib, "Mfplat.lib" )

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
}

HRESULT mfStartup()
{
	static Startup s_startup;
	return s_startup.startup();
}