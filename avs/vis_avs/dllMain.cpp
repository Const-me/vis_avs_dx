#include "stdafx.h"

BOOL __stdcall interopLibDllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved );

BOOL __stdcall DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	return interopLibDllMain( hModule, ul_reason_for_call, lpReserved );
}