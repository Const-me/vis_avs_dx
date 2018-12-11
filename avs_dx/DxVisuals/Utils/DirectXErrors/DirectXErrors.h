#pragma once

// Get error description from HRESULT code, e.g. "The caller did not supply a sufficiently large buffer." Returns nullptr for unknown codes. The data doesn't include Win32 errors, FormatMessage API already knows them.
const char* getDxErrorDescriptionA( HRESULT hr );

#ifdef DXERR_ATL_STRING

CStringA formatDxMessageA( HRESULT hr );
CStringW formatDxMessageW( HRESULT hr );

#elif defined DXERR_STD_STRING

#include <string>
std::string formatDxMessageA( HRESULT hr );
std::wstring formatDxMessageW( HRESULT hr );

#endif