#include "stdafx.h"

class CModule : public CAtlDllModuleT<CModule> { };

CModule _AtlModule;

// Apparently, VC++ linker only uses DllMain from the main DLL project, ignoring what's in the static libraries.
// That's why calling this explicitly, from vis_avs\dllMain.cpp
BOOL __stdcall interopLibDllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	if( DLL_PROCESS_ATTACH == ul_reason_for_call )
		DisableThreadLibraryCalls( hModule );
	return _AtlModule.DllMain( ul_reason_for_call, lpReserved );
}