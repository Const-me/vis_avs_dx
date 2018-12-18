#include "stdafx.h"

class CModule : public CAtlDllModuleT<CModule> { };

CModule _AtlModule;

BOOL __stdcall DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	// This function is not linked and not called. Probably good idea to fix, maybe with /ENTRY linker switch:
	// https://docs.microsoft.com/en-us/cpp/build/reference/entry-entry-point-symbol?view=vs-2017

	if( DLL_PROCESS_ATTACH == ul_reason_for_call )
		DisableThreadLibraryCalls( hModule );
	return _AtlModule.DllMain( ul_reason_for_call, lpReserved );
}